#include "RandomHeightmapGenerator.hpp"

#include <numbers>

#include "MaskGenerator.hpp"
#include "RandomUniformInt.hpp"
#include "RandomUniformReal.hpp"
#include "RandomBinary.hpp"

#include "Logger.hpp"

pcb::RandomHeightmapGenerator::RandomHeightmapGenerator(int mapWidth, int mapHeight) : mapWidth(mapWidth), mapHeight(mapHeight), noiseGenerator() {
	properties.amountOfLayersBounds = { 1, 10 };
	properties.noiseOffsetValueBounds = { 1, 10 };
	properties.absoluteNoiseValuesChance = 0.33;

	properties.applyMaskOnLayerChance = 0.33;
	properties.maskAmountOfLayersBounds = { 1, 3 };
	properties.maskOffsetMultiplicationValueBounds = { 0.0, 1.0 };
	properties.maskRadiusMultiplicationValueBounds = { 0.25, 1.0 };
	properties.maskFalloffMultiplicationValueBounds = { 0.1, 1.0 };
	properties.maskAbsoluteNoiseValuesChance = 0.33;

	properties.applyFinalMaskChance = 0.33;
	properties.finalMaskAmountOfLayersBounds = { 2, 5 };
	properties.finalMaskOffsetMultiplicationValueBounds = { 0.0, 1.0 };
	properties.finalMaskRadiusMultiplicationValueBounds = { 0.25, 1.0 };
	properties.finalMaskFalloffMultiplicationValueBounds = { 0.3, 1.0 };
	properties.finalMaskCompositeMaskShapesDistanceMultiplierBounds = { 0.5, 1.0 };

	properties.adjustmentLoweringThresholds = { 50, 230 };
	properties.adjustmentLoweringValue = 30;
	properties.adjustmentScaleDownAmplitudeThreshold = 220;
	properties.adjustmentScaleDownAmplitudeValueBounds = { 0.45, 0.75 };
	properties.adjustmentScaleUpAmplitudeThreshold = 100;
	properties.adjustmentScaleUpAmplitudeValueBounds = { 1.5, 2.2 };
}

pcb::RandomHeightmapGenerator::RandomHeightmapGenerator(int mapWidth, int mapHeight, const RandomGenerationControlProperties& properties) : mapWidth(mapWidth), mapHeight(mapHeight), properties(properties), noiseGenerator() {}

// TODO: Rotation of layers (lack of rotation is especially noticeable with 'stretched' noise).
pcb::LayeredHeightmap* pcb::RandomHeightmapGenerator::generateNew() const {
	LayeredHeightmap* heightmap = generateLayeredHeightmapNew();
	adjustLayeredHeightmap(heightmap);

	return heightmap;
}

pcb::LayeredHeightmap* pcb::RandomHeightmapGenerator::generateLayeredHeightmapNew() const {
	LayeredHeightmap* heightmap = new LayeredHeightmap(mapWidth, mapHeight);
	std::vector<HeightmapLayer> layers = generateLayers();

	for (const HeightmapLayer& layer : layers) {
		heightmap->addLayer(layer);
	}

	return heightmap;
}

std::vector<pcb::HeightmapLayer> pcb::RandomHeightmapGenerator::generateLayers() const {
	std::vector<LayerMode> layerModes = generateLayerModes();
	std::vector<HeightmapLayer> layers;
	Logger logger;

	for (int i = 0; i < layerModes.size(); i++) {
		logger << "Layer index " << i << "\n";
		layers.emplace_back(generateLayer(i, layerModes.at(i)));
	}

	RandomBinary finalMaskChanceValues(properties.applyFinalMaskChance);
	bool applyFinalMask = finalMaskChanceValues.generate();
	if (applyFinalMask) {
		Heightmap finalMask = generateFinalMask();
		layers.emplace_back(finalMask, LayerMode::Mask);
	}

	return layers;
}

std::vector<pcb::LayerMode> pcb::RandomHeightmapGenerator::generateLayerModes() const {
	RandomUniformInt layerAmountValues(properties.amountOfLayersBounds);
	int numberOfLayers = layerAmountValues.generate();
	std::vector<LayerData*> layerData{ new AdditionLayerData };

	for (int i = 0; i < numberOfLayers; i++) {
		const std::vector<LayerMode> allowedModes = layerData.back()->getAllowedNextModes(layerData);
		int amountOfAvailableModes = allowedModes.size();
		RandomUniformInt layerModeValues(0, amountOfAvailableModes - 1);
		int layerModeIndex = layerModeValues.generate();
		LayerMode layerMode = allowedModes.at(layerModeIndex);

		switch (layerMode) {
		case LayerMode::Addition:
			layerData.emplace_back(new AdditionLayerData());
			break;
		case LayerMode::Subtraction:
			layerData.emplace_back(new SubtractionLayerData());
			break;
		case LayerMode::Mask:
			layerData.emplace_back(new MaskLayerData());
			break;
		}
	}

	std::vector<LayerMode> layerModes;
	for (const LayerData* layerDataElement : layerData) {
		layerModes.emplace_back(layerDataElement->getMode());
	}

	for (int i = 0; i < layerData.size(); i++) {
		delete layerData.at(i);
	}
	layerData.clear();

	return layerModes;
}

pcb::HeightmapLayer pcb::RandomHeightmapGenerator::generateLayer(int layerIndex, LayerMode layerMode) const {
	int symmetricalFrequency = 128 - (10 * layerIndex);
	int noiseOffsetX = generateNoiseOffset();
	int noiseOffsetY = generateNoiseOffset();

	RandomBinary absoluteNoiseChance(properties.absoluteNoiseValuesChance);
	bool generateAbsoluteNoise = absoluteNoiseChance.generate();

	std::vector<HeightmapLayer> layerTempScopingSolution;		// TODO: This is a temporary solution to fix a scoping issue. Implement proper solution!
	if (generateAbsoluteNoise) {
		layerTempScopingSolution.emplace_back(generateAbsoluteNoiseHeightmap(symmetricalFrequency, symmetricalFrequency, noiseOffsetX, noiseOffsetY), layerMode);
	}
	else {
		layerTempScopingSolution.emplace_back(generateDefaultNoiseHeightmap(symmetricalFrequency, symmetricalFrequency, noiseOffsetX, noiseOffsetY), layerMode);
	}

	HeightmapLayer layer(layerTempScopingSolution.back().heightmap, layerTempScopingSolution.back().mode);
	layerTempScopingSolution.clear();
	
	if (layerMode == LayerMode::Mask) {
		layer.heightmap.raise(layerIndex * 5);
	}
	else {
		layer.heightmap.scale(0.5f - (layerIndex * 0.04));
	}	

	RandomBinary maskChance(properties.applyMaskOnLayerChance);
	bool applyMask = maskChance.generate();
	if (applyMask) {
		Heightmap mask = generateMask();
		layer.heightmap.mask(mask);
	}

	return layer;
}

int pcb::RandomHeightmapGenerator::generateNoiseOffset() const {
	RandomUniformInt noiseOffsetValues(properties.noiseOffsetValueBounds);
	return noiseOffsetValues.generate() * noiseOffsetValues.generate();
}

pcb::Heightmap pcb::RandomHeightmapGenerator::generateDefaultNoiseHeightmap(double noiseSamplingFrequencyX, double noiseSamplingFrequencyY, double xOffset, double yOffset) const {
	unsigned char* noiseMap = new unsigned char[mapWidth * mapHeight];
	const double samplingDistanceX = 1.0 / noiseSamplingFrequencyX;
	const double samplingDistanceY = 1.0 / noiseSamplingFrequencyY;

	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			double noiseInputX = xOffset + (x * samplingDistanceX);
			double noiseInputY = yOffset + (y * samplingDistanceY);
			noiseMap[(y * mapWidth) + x] = static_cast<unsigned char>(generateElevationForNoiseCoordinates(noiseInputX, noiseInputY));
		}
	}

	pcb::Heightmap heightmap(mapWidth, mapHeight, noiseMap);
	delete[] noiseMap;

	return heightmap;
}

pcb::Heightmap pcb::RandomHeightmapGenerator::generateAbsoluteNoiseHeightmap(double noiseSamplingFrequencyX, double noiseSamplingFrequencyY, double xOffset, double yOffset) const {
	unsigned char* noiseMap = new unsigned char[mapWidth * mapHeight];
	const double samplingDistanceX = 1.0 / noiseSamplingFrequencyX;
	const double samplingDistanceY = 1.0 / noiseSamplingFrequencyY;

	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			double noiseInputX = xOffset + (x * samplingDistanceX);
			double noiseInputY = yOffset + (y * samplingDistanceY);
			noiseMap[(y * mapWidth) + x] = static_cast<unsigned char>(generateElevationForNoiseCoordinatesWithAbsoluteModifier(noiseInputX, noiseInputY));
		}
	}

	pcb::Heightmap heightmap(mapWidth, mapHeight, noiseMap);
	delete[] noiseMap;

	return heightmap;
}

double pcb::RandomHeightmapGenerator::generateElevationForNoiseCoordinates(double x, double y) const {
	return generateElevationFromNoiseValue(noiseGenerator.getValueForCoordinate(x, y));
}

double pcb::RandomHeightmapGenerator::generateElevationForNoiseCoordinatesWithAbsoluteModifier(double x, double y) const {
	return generateElevationFromNoiseValue(noiseGenerator.getAbsoluteValueForCoordinate(x, y));
}

double pcb::RandomHeightmapGenerator::generateElevationFromNoiseValue(double noiseValue) const {
	return std::round(127.5 * (1 + noiseValue));
}

pcb::Heightmap pcb::RandomHeightmapGenerator::generateMask() const {
	MaskGenerator maskGenerator;
	RandomBinary binaryChance;
	RandomUniformInt layerCountValues(properties.maskAmountOfLayersBounds);
	int amountOfLayers = layerCountValues.generate();
	RandomUniformReal offsetMultiplicationValues(properties.maskOffsetMultiplicationValueBounds);
	RandomUniformReal radiusMultiplicationValues(properties.maskRadiusMultiplicationValueBounds);
	RandomUniformReal falloffMultiplicationValues(properties.maskFalloffMultiplicationValueBounds);
	std::vector<Heightmap> maskLayers;
	RandomUniformInt maskTypeValues(0, 2);

	Logger logger;
	logger << "Amount of mask layers: " << amountOfLayers << "\n";

	for (int i = 0; i < amountOfLayers; i++) {
		int offsetXModifier = binaryChance.generate() == true ? 1 : -1;
		int offsetYModifier = binaryChance.generate() == true ? 1 : -1;
		int offsetX = offsetMultiplicationValues.generate() * (offsetXModifier * 0.5f * mapWidth);
		int offsetY = offsetMultiplicationValues.generate() * (offsetYModifier * 0.5f * mapHeight);
		int unaffectedRadiusX = radiusMultiplicationValues.generate() * (0.25f * mapWidth);
		int unaffectedRadiusY = radiusMultiplicationValues.generate() * (0.25f * mapHeight);
		int falloffWidth = falloffMultiplicationValues.generate() * (0.25f * mapWidth);

		int maskTypeValue = maskTypeValues.generate();
		if (maskTypeValue == 0) {
			logger << "MaskType: Circle\n";
			maskLayers.emplace_back(maskGenerator.generateCircleLinearFalloffMask(mapWidth, mapHeight, unaffectedRadiusX, falloffWidth, offsetX, offsetY));
		}
		else if(maskTypeValue == 1) {
			logger << "MaskType: Rectangle\n";
			maskLayers.emplace_back(maskGenerator.generateRectangleLinearFalloffMask(mapWidth, mapHeight, unaffectedRadiusX, unaffectedRadiusY, falloffWidth, offsetX, offsetY));
		}
		else {
			logger << "MaskType: Noise - ";
			RandomUniformInt noiseFrequencyValues(1, 2);
			int frequencyX = 128 * (noiseFrequencyValues.generate() / 2.0f);
			int frequencyY = 128 * (noiseFrequencyValues.generate() / 2.0f);
			RandomBinary absoluteNoiseChance(properties.maskAbsoluteNoiseValuesChance);
			bool generateAbsoluteNoise = absoluteNoiseChance.generate();

			if (generateAbsoluteNoise) {
				logger << "Absolute\n";
				maskLayers.emplace_back(generateAbsoluteNoiseHeightmap(frequencyX, frequencyY, offsetX, offsetY));
			}
			else {
				logger << "Default\n";
				maskLayers.emplace_back(generateDefaultNoiseHeightmap(frequencyX, frequencyY, offsetX, offsetY));
			}			
		}		
	}

	Heightmap mask = maskLayers.front();
	for (int i = 1; i < maskLayers.size(); i++) {
		mask.add(maskLayers.at(i));
	}

	bool invertMask = binaryChance.generate();
	if (invertMask) {
		logger << "Inverted mask!\n";
		mask.invert();
	}

	logger << "\n";

	return mask;
}

pcb::Heightmap pcb::RandomHeightmapGenerator::generateFinalMask() const {
	MaskGenerator maskGenerator;
	RandomBinary binaryChance;
	RandomUniformInt layerCountValues(properties.finalMaskAmountOfLayersBounds);
	int amountOfLayers = layerCountValues.generate();
	RandomUniformReal offsetMultiplicationValues(properties.finalMaskOffsetMultiplicationValueBounds);
	RandomUniformReal radiusMultiplicationValues(properties.finalMaskRadiusMultiplicationValueBounds);
	RandomUniformReal falloffMultiplicationValues(properties.finalMaskFalloffMultiplicationValueBounds);
	std::vector<Heightmap> maskLayers;	
	RandomBinary maskTypeSelector;

	std::vector<MaskShapeData> maskShapeData;
	Logger logger;

	logger << "\n\n";

	for (int i = 0; i < amountOfLayers; i++) {
		int offsetXModifier = binaryChance.generate() == true ? 1 : -1;
		int offsetYModifier = binaryChance.generate() == true ? 1 : -1;
		int offsetX = offsetMultiplicationValues.generate() * (offsetXModifier * 0.5f * mapWidth);
		int offsetY = offsetMultiplicationValues.generate() * (offsetYModifier * 0.5f * mapHeight);
		int unaffectedRadiusX = radiusMultiplicationValues.generate() * (0.25f * mapWidth);
		int unaffectedRadiusY = radiusMultiplicationValues.generate() * (0.25f * mapHeight);
		int falloffWidth = falloffMultiplicationValues.generate() * (0.25f * mapWidth);

		if (maskShapeData.size() > 0) {
			RandomUniformInt indexValues(0, maskShapeData.size() - 1);
			int previousShapeIndex = indexValues.generate();
			MaskShapeData previousShape = maskShapeData.at(previousShapeIndex);

			RandomUniformReal angleValues(0.0, 2 * std::numbers::pi);
			double angleFromPreviousShapeToNewShape = angleValues.generate();
			double cosAngle = std::cos(angleFromPreviousShapeToNewShape);
			double sinAngle = std::sin(angleFromPreviousShapeToNewShape);

			int minimumRadius = std::min(unaffectedRadiusX, unaffectedRadiusY);
			int maximumDistance = minimumRadius + falloffWidth + std::min(previousShape.unaffectedRadiusX, previousShape.unaffectedRadiusY);
			RandomUniformReal distanceMultiplierValues(properties.finalMaskCompositeMaskShapesDistanceMultiplierBounds);
			double distanceMultiplier = distanceMultiplierValues.generate();
			offsetX = (distanceMultiplier * cosAngle * maximumDistance) + previousShape.offsetX;
			offsetY = (distanceMultiplier * sinAngle * maximumDistance) + previousShape.offsetY;

			logger << "\n\n";
			logger << "PreviousShapeIndex: " << previousShapeIndex << "\n";
			logger << "AngleFromPreviousShapeToNewShape: " << angleFromPreviousShapeToNewShape * (180 / std::numbers::pi) << "\n";
			logger << "cosAngle: " << cosAngle << "\n";
			logger << "sinAngle: " << sinAngle << "\n";
			logger << "minimumRadius: " << minimumRadius << "\n";
			logger << "maximumDistance: " << maximumDistance << "\n";
			logger << "distanceMultiplier: " << distanceMultiplier << "\n";
			logger << "offsetFromPrevious: " << (distanceMultiplier * cosAngle * maximumDistance) << "; " << (distanceMultiplier * sinAngle * maximumDistance) << "\n";
			logger << "offset: " << offsetX << "; " << offsetY << "\n";

			// TODO: Check if offsets are in map range!?
		}
		else {
			logger << "offset: " << offsetX << "; " << offsetY << "\n";
			logger << "RadiusX: " << unaffectedRadiusX << ", RadiusY: " << unaffectedRadiusY << "\n";
			logger << "Falloff: " << falloffWidth << "\n";
		}

		bool maskTypeIsCircle = maskTypeSelector.generate();
		if (maskTypeIsCircle) {
			logger << "MaskType: Circle\n";
			maskLayers.emplace_back(maskGenerator.generateCircleLinearFalloffMask(mapWidth, mapHeight, unaffectedRadiusX, falloffWidth, offsetX, offsetY));			
		}
		else {
			logger << "MaskType: Rectangle\n";
			maskLayers.emplace_back(maskGenerator.generateRectangleLinearFalloffMask(mapWidth, mapHeight, unaffectedRadiusX, unaffectedRadiusY, falloffWidth, offsetX, offsetY));			
		}

		maskShapeData.push_back({ offsetX, offsetY, unaffectedRadiusX, unaffectedRadiusY, falloffWidth });
	}

	Heightmap mask = maskLayers.front();
	for (int i = 1; i < maskLayers.size(); i++) {
		mask.add(maskLayers.at(i));
	}

	if (binaryChance.generate() == true) {
		mask.invert();
	}

	logger << "\n";

	return mask;
}

void pcb::RandomHeightmapGenerator::adjustLayeredHeightmap(LayeredHeightmap* const heightmap) const {
	Logger logger;

	if (heightmap->getLowestElevation() > properties.adjustmentLoweringThresholds.lower && heightmap->getHighestElevation() > properties.adjustmentLoweringThresholds.upper) {
		logger << "ADJUSTMENT: Lowering heightmap\n";
		heightmap->lower(properties.adjustmentLoweringValue);
	}

	if (heightmap->getHighestElevation() - heightmap->getLowestElevation() > properties.adjustmentScaleDownAmplitudeThreshold) {
		logger << "ADJUSTMENT: Scaling down heightmap amplitude\n";
		RandomUniformReal scaleAmplitudeFactorValues(properties.adjustmentScaleDownAmplitudeValueBounds);
		heightmap->scaleAmplitude(scaleAmplitudeFactorValues.generate());
	}
	else if (heightmap->getHighestElevation() - heightmap->getLowestElevation() < properties.adjustmentScaleUpAmplitudeThreshold) {
		logger << "ADJUSTMENT: Scaling up heightmap amplitude\n";
		RandomUniformReal scaleAmplitudeFactorValues(properties.adjustmentScaleUpAmplitudeValueBounds);
		heightmap->scaleAmplitude(scaleAmplitudeFactorValues.generate());
	}

	logger << "\n";
}

#pragma region Layer_Data
pcb::RandomHeightmapGenerator::LayerData::LayerData(const LayerMode layerMode) : layerMode(layerMode) {}

const std::vector<pcb::LayerMode> pcb::RandomHeightmapGenerator::LayerData::getAllowedNextModes(const std::vector<LayerData*> previousLayers) const {
	return determineAllowedNextModes(previousLayers);
}

const pcb::LayerMode pcb::RandomHeightmapGenerator::LayerData::getMode() const {
	return layerMode;
}

int pcb::RandomHeightmapGenerator::LayerData::countAmountOfConsecutiveLayerModesAtEnd(const std::vector<LayerData*> previousLayers, pcb::LayerMode layerMode) const {
	int count = 0;
	for (int i = previousLayers.size() - 1; i >= 0; i--) {
		if (previousLayers.at(i)->layerMode == layerMode) {
			count++;
		}
		else {
			break;
		}
	}

	return count;
}

pcb::RandomHeightmapGenerator::AdditionLayerData::AdditionLayerData() : LayerData(LayerMode::Addition) {}

std::vector<pcb::LayerMode> pcb::RandomHeightmapGenerator::AdditionLayerData::determineAllowedNextModes(const std::vector<LayerData*> previousLayers) const {
	std::vector<LayerMode> allowedNextModes;
	if (previousLayers.back()->getMode() != LayerMode::Addition) {
		allowedNextModes.emplace_back(LayerMode::Addition);
	}

	allowedNextModes.emplace_back(LayerMode::Subtraction);
	allowedNextModes.emplace_back(LayerMode::Mask);

	return allowedNextModes;
}

pcb::RandomHeightmapGenerator::SubtractionLayerData::SubtractionLayerData() : LayerData(LayerMode::Subtraction) {}

std::vector<pcb::LayerMode> pcb::RandomHeightmapGenerator::SubtractionLayerData::determineAllowedNextModes(const std::vector<LayerData*> previousLayers) const {
	std::vector<LayerMode> allowedNextModes;
	allowedNextModes.emplace_back(LayerMode::Addition);

	if (previousLayers.back()->getMode() == LayerMode::Addition) {
		allowedNextModes.emplace_back(LayerMode::Mask);
	}	

	return allowedNextModes;
}

pcb::RandomHeightmapGenerator::MaskLayerData::MaskLayerData() : LayerData(LayerMode::Mask) {}

std::vector<pcb::LayerMode> pcb::RandomHeightmapGenerator::MaskLayerData::determineAllowedNextModes(std::vector<LayerData*> previousLayers) const {
	std::vector<LayerMode> allowedNextModes;
	allowedNextModes.emplace_back(LayerMode::Addition);

	if (previousLayers.back()->getMode() == LayerMode::Addition) {
		allowedNextModes.emplace_back(LayerMode::Subtraction);
	}	

	return allowedNextModes;
}
#pragma endregion