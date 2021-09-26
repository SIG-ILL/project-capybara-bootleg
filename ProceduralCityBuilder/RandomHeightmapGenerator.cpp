#include "RandomHeightmapGenerator.hpp"

#include <numbers>
#include <array>
#include <memory>

#include "MaskGenerator.hpp"
#include "RandomUniformInt.hpp"
#include "RandomUniformReal.hpp"
#include "RandomBinary.hpp"

#include "Logger.hpp"

pcb::RandomHeightmapGenerator::RandomHeightmapGenerator(int mapWidth, int mapHeight) : mapWidth(mapWidth), mapHeight(mapHeight), noiseGenerator() {
	properties = getDefaultControlProperties();
}

pcb::RandomHeightmapGenerator::RandomHeightmapGenerator(int mapWidth, int mapHeight, const RandomGenerationControlProperties& properties) : mapWidth(mapWidth), mapHeight(mapHeight), properties(properties), noiseGenerator() {}

// TODO: Rotation of layers (lack of rotation is especially noticeable with 'stretched' noise).
pcb::LayeredHeightmap pcb::RandomHeightmapGenerator::generate() const {
	LayeredHeightmap heightmap = generateLayeredHeightmap();
	adjustLayeredHeightmap(heightmap);

	return heightmap;
}

pcb::RandomGenerationControlProperties pcb::RandomHeightmapGenerator::getDefaultControlProperties() const {
	RandomGenerationControlProperties defaultProperties;

	defaultProperties.amountOfLayersBounds = { 1, 10 };
	defaultProperties.layerBaseNoiseFrequency = 128;
	defaultProperties.layerNoiseFrequencyAdditionalLayerModifier = 10;
	defaultProperties.layerScalingBaseValue = 0.5;
	defaultProperties.layerScalingAdditionalLayerModifier = 0.04;
	defaultProperties.noiseOffsetValueBounds = { 1, 10 };
	defaultProperties.defaultNoiseInversionChance = 0.5;
	defaultProperties.absoluteNoiseChance = 0.33;
	defaultProperties.absoluteNoiseFrequencyModifier = 2;
	defaultProperties.absoluteNoiseInversionChance = 0.5;

	defaultProperties.applyMaskOnLayerChance = 0.33;
	defaultProperties.maskAmountOfLayersBounds = { 1, 3 };
	defaultProperties.maskOffsetMultiplicationValueBounds = { 0.0, 1.0 };
	defaultProperties.maskRadiusMultiplicationValueBounds = { 0.25, 1.0 };
	defaultProperties.maskFalloffMultiplicationValueBounds = { 0.1, 1.0 };
	defaultProperties.maskDefaultNoiseInversionChance = 0.5;
	defaultProperties.maskAbsoluteNoiseChance = 0.33;
	defaultProperties.maskAbsoluteNoiseFrequencyModifier = 2;
	defaultProperties.maskAbsoluteNoiseInversionChance = 0.5;

	defaultProperties.applyFinalMaskChance = 0.33;
	defaultProperties.finalMaskAmountOfLayersBounds = { 2, 5 };
	defaultProperties.finalMaskOffsetMultiplicationValueBounds = { 0.0, 1.0 };
	defaultProperties.finalMaskRadiusMultiplicationValueBounds = { 0.25, 1.0 };
	defaultProperties.finalMaskFalloffMultiplicationValueBounds = { 0.3, 1.0 };
	defaultProperties.finalMaskCompositeMaskShapesDistanceMultiplierBounds = { 0.5, 1.0 };

	defaultProperties.adjustmentLoweringThresholds = { 50, 230 };
	defaultProperties.adjustmentLoweringValue = 30;
	defaultProperties.adjustmentScaleDownAmplitudeThreshold = 220;
	defaultProperties.adjustmentScaleDownAmplitudeValueBounds = { 0.45, 0.75 };
	defaultProperties.adjustmentScaleUpAmplitudeThreshold = 100;
	defaultProperties.adjustmentScaleUpAmplitudeValueBounds = { 1.5, 2.2 };

	return defaultProperties;
}

void pcb::RandomHeightmapGenerator::setControlProperties(const RandomGenerationControlProperties& properties) {
	this->properties = properties;
}

pcb::LayeredHeightmap pcb::RandomHeightmapGenerator::generateLayeredHeightmap() const {
	LayeredHeightmap heightmap(mapWidth, mapHeight);
	std::vector<HeightmapLayer> layers = generateLayers();

	for (const HeightmapLayer& layer : layers) {
		heightmap.addLayer(layer);
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
		logger << "\n";
	}

	RandomBinary finalMaskChanceValues(properties.applyFinalMaskChance);
	bool applyFinalMask = finalMaskChanceValues.generate();
	if (applyFinalMask) {
		logger << "Layer index " << layerModes.size() << " - Final mask\n";
		Heightmap finalMask = generateFinalMask();
		layers.emplace_back(finalMask, LayerMode::Mask);
	}

	return layers;
}

std::vector<pcb::LayerMode> pcb::RandomHeightmapGenerator::generateLayerModes() const {
	RandomUniformInt layerAmountValues(properties.amountOfLayersBounds);
	int numberOfLayers = layerAmountValues.generate();
	std::vector<std::unique_ptr<LayerData>> layerData;
	layerData.push_back(std::make_unique<AdditionLayerData>());

	for (int i = 0; i < numberOfLayers; i++) {
		const std::vector<LayerMode> allowedModes = layerData.back()->getAllowedNextModes(layerData);
		int amountOfAvailableModes = allowedModes.size();
		RandomUniformInt layerModeValues(0, amountOfAvailableModes - 1);
		int layerModeIndex = layerModeValues.generate();
		LayerMode layerMode = allowedModes.at(layerModeIndex);

		switch (layerMode) {
		case LayerMode::Addition:
			layerData.push_back(std::make_unique<AdditionLayerData>());
			break;
		case LayerMode::Subtraction:
			layerData.push_back(std::make_unique<SubtractionLayerData>());
			break;
		case LayerMode::Mask:
			layerData.push_back(std::make_unique<MaskLayerData>());
			break;
		}
	}

	std::vector<LayerMode> layerModes;
	for (const std::unique_ptr<LayerData>& layerDataElement : layerData) {
		layerModes.emplace_back(layerDataElement->getMode());
	}

	layerData.clear();

	return layerModes;
}

pcb::HeightmapLayer pcb::RandomHeightmapGenerator::generateLayer(int layerIndex, LayerMode layerMode) const {
	double symmetricalFrequency = properties.layerBaseNoiseFrequency - (properties.layerNoiseFrequencyAdditionalLayerModifier * layerIndex);
	int noiseOffsetX = generateNoiseOffset();
	int noiseOffsetY = generateNoiseOffset();

	RandomBinary absoluteNoiseChance(properties.absoluteNoiseChance);
	bool generateAbsoluteNoise = absoluteNoiseChance.generate();

	Logger logger;
	logger << "Generating layer, noise type: ";

	Heightmap heightmap(0, 0);		// 'Empty' object that gets assigned values in the if/else block below.
	bool invertLayer = false;
	if (generateAbsoluteNoise) {
		logger << "Absolute\n";
		symmetricalFrequency *= properties.absoluteNoiseFrequencyModifier;
		heightmap = generateAbsoluteNoiseHeightmap(symmetricalFrequency, symmetricalFrequency, noiseOffsetX, noiseOffsetY);

		RandomBinary invertChance(properties.absoluteNoiseInversionChance);
		invertLayer = invertChance.generate();
	}
	else {
		logger << "Default\n";
		heightmap = generateDefaultNoiseHeightmap(symmetricalFrequency, symmetricalFrequency, noiseOffsetX, noiseOffsetY);

		RandomBinary invertChance(properties.defaultNoiseInversionChance);
		invertLayer = invertChance.generate();
	}

	if (invertLayer) {
		logger << "Inverting noise layer\n";
		heightmap.invert();
	}

	HeightmapLayer layer(heightmap, layerMode);
	
	if (layerMode == LayerMode::Mask) {
		unsigned char raiseAmount = layerIndex * 5;
		logger << "Raising layer by " << raiseAmount << " as it is a mask\n";
		layer.heightmap.raise(raiseAmount);
	}
	else {
		double scalingFactor = properties.layerScalingBaseValue - (layerIndex * properties.layerScalingAdditionalLayerModifier);
		logger << "Scaling layer by " << scalingFactor << " as it is not a mask\n";
		layer.heightmap.scale(scalingFactor);
	}	

	RandomBinary maskChance(properties.applyMaskOnLayerChance);
	bool applyMask = maskChance.generate();
	if (applyMask) {
		logger << "Applying mask to layer\n";
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
	std::vector<unsigned char> noiseMap;
	noiseMap.reserve(mapWidth * mapHeight);
	const double samplingDistanceX = 1.0 / noiseSamplingFrequencyX;
	const double samplingDistanceY = 1.0 / noiseSamplingFrequencyY;

	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			double noiseInputX = xOffset + (x * samplingDistanceX);
			double noiseInputY = yOffset + (y * samplingDistanceY);
			noiseMap.push_back(static_cast<unsigned char>(generateElevationForNoiseCoordinates(noiseInputX, noiseInputY)));
		}
	}

	pcb::Heightmap heightmap(mapWidth, mapHeight, noiseMap);

	return heightmap;
}

pcb::Heightmap pcb::RandomHeightmapGenerator::generateAbsoluteNoiseHeightmap(double noiseSamplingFrequencyX, double noiseSamplingFrequencyY, double xOffset, double yOffset) const {
	std::vector<unsigned char> noiseMap;
	noiseMap.reserve(mapWidth * mapHeight);
	const double samplingDistanceX = 1.0 / noiseSamplingFrequencyX;
	const double samplingDistanceY = 1.0 / noiseSamplingFrequencyY;

	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			double noiseInputX = xOffset + (x * samplingDistanceX);
			double noiseInputY = yOffset + (y * samplingDistanceY);
			noiseMap.push_back(static_cast<unsigned char>(generateElevationForNoiseCoordinatesWithAbsoluteModifier(noiseInputX, noiseInputY)));
		}
	}

	pcb::Heightmap heightmap(mapWidth, mapHeight, noiseMap);

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
	RandomUniformInt maskTypeValues(0, 3);
	std::array<GradientDirection, 4> gradientDirections = { GradientDirection::Up, GradientDirection::Left, GradientDirection::Down, GradientDirection::Right };

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
		else if (maskTypeValue == 2) {		// TODO: If there already is a linear gradient in the vector, don't add the gradient that goes the opposite way as they will cancel each other out and producte a mask that doesn't do anything (or masks *everything* if it's inverted).
			logger << "MaskType: Linear Gradient";
			RandomUniformInt gradientDirectionValues(0, 3);
			int gradientDirectionIndex = gradientDirectionValues.generate();
			logger << " - DirectionIndex " << gradientDirectionIndex << "\n";
			maskLayers.emplace_back(maskGenerator.generateLinearGradientMask(mapWidth, mapHeight, gradientDirections[gradientDirectionIndex]));
		}
		else {
			logger << "MaskType: Noise - ";
			RandomUniformInt noiseFrequencyValues(1, 2);
			int frequencyX = 128 * (noiseFrequencyValues.generate() / 2.0f);
			int frequencyY = 128 * (noiseFrequencyValues.generate() / 2.0f);
			RandomBinary absoluteNoiseChance(properties.maskAbsoluteNoiseChance);
			bool generateAbsoluteNoise = absoluteNoiseChance.generate();
			bool invertLayer = false;

			if (generateAbsoluteNoise) {
				logger << "Absolute\n";
				frequencyX *= properties.maskAbsoluteNoiseFrequencyModifier;
				frequencyY *= properties.maskAbsoluteNoiseFrequencyModifier;
				maskLayers.emplace_back(generateAbsoluteNoiseHeightmap(frequencyX, frequencyY, offsetX, offsetY));

				RandomBinary invertChance(properties.maskAbsoluteNoiseInversionChance);
				invertLayer = invertChance.generate();
			}
			else {
				logger << "Default\n";
				maskLayers.emplace_back(generateDefaultNoiseHeightmap(frequencyX, frequencyY, offsetX, offsetY));

				RandomBinary invertChance(properties.maskDefaultNoiseInversionChance);
				invertLayer = invertChance.generate();
			}

			if (invertLayer) {
				logger << "Inverting mask noise layer\n";
				maskLayers.back().invert();
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

	bool invertMask = binaryChance.generate();
	if (invertMask) {
		logger << "Inverted mask!\n";
		mask.invert();
	}

	logger << "\n";

	return mask;
}

void pcb::RandomHeightmapGenerator::adjustLayeredHeightmap(LayeredHeightmap& heightmap) const {
	Logger logger;

	if (heightmap.getLowestElevation() > properties.adjustmentLoweringThresholds.lower && heightmap.getHighestElevation() > properties.adjustmentLoweringThresholds.upper) {
		logger << "ADJUSTMENT: Lowering heightmap\n";
		heightmap.lower(properties.adjustmentLoweringValue);
	}

	if (heightmap.getHighestElevation() - heightmap.getLowestElevation() > properties.adjustmentScaleDownAmplitudeThreshold) {
		logger << "ADJUSTMENT: Scaling down heightmap amplitude\n";
		RandomUniformReal scaleAmplitudeFactorValues(properties.adjustmentScaleDownAmplitudeValueBounds);
		heightmap.scaleAmplitude(scaleAmplitudeFactorValues.generate());
	}
	else if (heightmap.getHighestElevation() - heightmap.getLowestElevation() < properties.adjustmentScaleUpAmplitudeThreshold) {
		logger << "ADJUSTMENT: Scaling up heightmap amplitude\n";
		RandomUniformReal scaleAmplitudeFactorValues(properties.adjustmentScaleUpAmplitudeValueBounds);
		heightmap.scaleAmplitude(scaleAmplitudeFactorValues.generate());
	}

	logger << "\n";
}

#pragma region Layer_Data
pcb::RandomHeightmapGenerator::LayerData::LayerData(const LayerMode layerMode) : layerMode(layerMode) {}

const std::vector<pcb::LayerMode> pcb::RandomHeightmapGenerator::LayerData::getAllowedNextModes(const std::vector<std::unique_ptr<LayerData>>& previousLayers) const {
	return determineAllowedNextModes(previousLayers);
}

const pcb::LayerMode pcb::RandomHeightmapGenerator::LayerData::getMode() const {
	return layerMode;
}

int pcb::RandomHeightmapGenerator::LayerData::countAmountOfConsecutiveLayerModesAtEnd(const std::vector<std::unique_ptr<LayerData>>& previousLayers, pcb::LayerMode layerMode) const {
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

std::vector<pcb::LayerMode> pcb::RandomHeightmapGenerator::AdditionLayerData::determineAllowedNextModes(const std::vector<std::unique_ptr<LayerData>>& previousLayers) const {
	std::vector<LayerMode> allowedNextModes;
	if (previousLayers.back()->getMode() != LayerMode::Addition) {
		allowedNextModes.push_back(LayerMode::Addition);
	}

	allowedNextModes.push_back(LayerMode::Subtraction);
	allowedNextModes.push_back(LayerMode::Mask);

	return allowedNextModes;
}

pcb::RandomHeightmapGenerator::SubtractionLayerData::SubtractionLayerData() : LayerData(LayerMode::Subtraction) {}

std::vector<pcb::LayerMode> pcb::RandomHeightmapGenerator::SubtractionLayerData::determineAllowedNextModes(const std::vector<std::unique_ptr<LayerData>>& previousLayers) const {
	std::vector<LayerMode> allowedNextModes;
	allowedNextModes.push_back(LayerMode::Addition);

	if (previousLayers.back()->getMode() == LayerMode::Addition) {
		allowedNextModes.push_back(LayerMode::Mask);
	}	

	return allowedNextModes;
}

pcb::RandomHeightmapGenerator::MaskLayerData::MaskLayerData() : LayerData(LayerMode::Mask) {}

std::vector<pcb::LayerMode> pcb::RandomHeightmapGenerator::MaskLayerData::determineAllowedNextModes(const std::vector<std::unique_ptr<LayerData>>& previousLayers) const {
	std::vector<LayerMode> allowedNextModes;
	allowedNextModes.push_back(LayerMode::Addition);

	if (previousLayers.back()->getMode() == LayerMode::Addition) {
		allowedNextModes.push_back(LayerMode::Subtraction);
	}	

	return allowedNextModes;
}
#pragma endregion