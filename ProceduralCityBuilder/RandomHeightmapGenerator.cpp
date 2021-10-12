#include "RandomHeightmapGenerator.hpp"

#include <numbers>
#include <array>
#include <memory>

#include "MaskGenerator.hpp"
#include "RandomUniformInt.hpp"
#include "RandomUniformReal.hpp"
#include "RandomBinary.hpp"

#include "Logger.hpp"

pcb::RandomHeightmapGenerator::RandomHeightmapGenerator(int mapWidth, int mapHeight) : mapWidth(mapWidth), mapHeight(mapHeight), noiseMapGenerator(mapWidth, mapHeight), absoluteNoiseMapGenerator(mapWidth, mapHeight) {
	properties = getDefaultControlProperties();
}

pcb::RandomHeightmapGenerator::RandomHeightmapGenerator(int mapWidth, int mapHeight, const RandomGenerationControlProperties& properties) : mapWidth(mapWidth), mapHeight(mapHeight), properties(properties), noiseMapGenerator(mapWidth, mapHeight), absoluteNoiseMapGenerator(mapWidth, mapHeight) {}

// TODO: Rotation of layers (lack of rotation is especially noticeable with 'stretched' noise).
std::unique_ptr<pcb::LayeredHeightmap> pcb::RandomHeightmapGenerator::generate() const {
	std::unique_ptr<LayeredHeightmap> heightmap = generateLayeredHeightmap();
	adjustLayeredHeightmap(*heightmap);

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

std::unique_ptr<pcb::LayeredHeightmap> pcb::RandomHeightmapGenerator::generateLayeredHeightmap() const {
	std::unique_ptr<LayeredHeightmap> heightmap = std::make_unique<LayeredHeightmap>(mapWidth, mapHeight);
	std::unique_ptr<std::vector<std::unique_ptr<HeightmapLayer>>> layers = generateLayers();

	for (int i = 0; i < layers->size(); i++) {
		heightmap->addLayer(std::move(layers->at(i)));
	}
	layers->clear();

	return heightmap;
}

std::unique_ptr<std::vector<std::unique_ptr<pcb::HeightmapLayer>>> pcb::RandomHeightmapGenerator::generateLayers() const {
	RandomUniformInt layerAmountValues(properties.amountOfLayersBounds);
	int amountOfLayers = layerAmountValues.generate();
	std::unique_ptr<std::vector<std::unique_ptr<LayerData>>> layerData = generateLayerData(amountOfLayers);
	std::unique_ptr<std::vector<std::unique_ptr<HeightmapLayer>>> layers = std::make_unique<std::vector<std::unique_ptr<HeightmapLayer>>>();
	Logger logger;

	for (int i = 0; i < layerData->size(); i++) {
		logger << "Layer index " << i << "\n";
		RandomBinary absoluteNoiseChance(properties.absoluteNoiseChance);
		bool generateAbsoluteNoise = absoluteNoiseChance.generate();

		NoiseMapGenerationParameters generationParameters = layerData->at(i)->getNoiseMapGenerationParameters();

		if (generateAbsoluteNoise) {
			generationParameters.symmetricalFrequency *= properties.absoluteNoiseFrequencyModifier;
			RandomBinary invertChance(properties.absoluteNoiseInversionChance);
			generationParameters.invert = invertChance.generate();

			logger << "Generating layer, noise type: Absolute\n";
			layers->push_back(generateNoiseLayer(absoluteNoiseMapGenerator, *(layerData->at(i))));
		}
		else {
			RandomBinary invertChance(properties.defaultNoiseInversionChance);
			generationParameters.invert = invertChance.generate();

			logger << "Generating layer, noise type: Default\n";
			layers->push_back(generateNoiseLayer(noiseMapGenerator, *(layerData->at(i))));
		}
		
		logger << "\n";
	}

	RandomBinary finalMaskChanceValues(properties.applyFinalMaskChance);
	bool applyFinalMask = finalMaskChanceValues.generate();
	if (applyFinalMask) {
		logger << "Layer index " << layerData->size() << " - Final mask\n";
		layers->push_back(std::make_unique<HeightmapLayer>(generateFinalMask() , LayerMode::Mask));
	}

	return layers;
}

std::unique_ptr<std::vector<std::unique_ptr<pcb::LayerData>>> pcb::RandomHeightmapGenerator::generateLayerData(int amountOfLayers) const {
	std::unique_ptr<std::vector<std::unique_ptr<LayerData>>> layerData = std::make_unique<std::vector<std::unique_ptr<LayerData>>>();
	layerData->push_back(std::make_unique<AdditionLayerData>(0, properties));
	for (int i = 0; i < (amountOfLayers - 1); i++) {
		layerData->push_back(layerData->back()->getRandomAllowedNextNode(*layerData, (i + 1), properties));
	}

	return layerData;
}

std::unique_ptr<pcb::HeightmapLayer> pcb::RandomHeightmapGenerator::generateNoiseLayer(const NoiseMapGenerator& generator, const LayerData& layerData) const {
	Logger logger;

	std::unique_ptr<Heightmap> heightmap;
	NoiseMapGenerationParameters generationParameters = layerData.getNoiseMapGenerationParameters();
	heightmap = generator.generate(generationParameters.symmetricalFrequency, generationParameters.symmetricalFrequency, generationParameters.noiseOffsetX, generationParameters.noiseOffsetY);

	if (generationParameters.invert) {
		logger << "Inverting noise layer\n";
		heightmap->invert();
	}

	LayerMode layerMode = layerData.getMode();
	int layerIndex = layerData.getIndex();
	std::unique_ptr<HeightmapLayer> layer = std::make_unique<HeightmapLayer>(std::move(heightmap), layerMode);
	if (layerMode == LayerMode::Mask) {
		unsigned char raiseAmount = 5 * layerIndex;
		logger << "Raising layer by " << raiseAmount << " as it is a mask\n";
		layer->heightmap->raise(raiseAmount);
	}
	else {
		double scalingFactor = properties.layerScalingBaseValue - (layerIndex * properties.layerScalingAdditionalLayerModifier);
		logger << "Scaling layer by " << scalingFactor << " as it is not a mask\n";
		layer->heightmap->scale(scalingFactor);
	}

	if (generationParameters.applyMask) {
		logger << "Applying mask to layer\n";
		std::unique_ptr<Heightmap> mask = generateMask();
		layer->heightmap->mask(*mask);
	}

	return layer;
}

std::unique_ptr<pcb::Heightmap> pcb::RandomHeightmapGenerator::generateMask() const {
	MaskGenerator maskGenerator;
	RandomBinary binaryChance;
	RandomUniformInt layerCountValues(properties.maskAmountOfLayersBounds);
	int amountOfLayers = layerCountValues.generate();
	RandomUniformReal offsetMultiplicationValues(properties.maskOffsetMultiplicationValueBounds);
	RandomUniformReal radiusMultiplicationValues(properties.maskRadiusMultiplicationValueBounds);
	RandomUniformReal falloffMultiplicationValues(properties.maskFalloffMultiplicationValueBounds);
	std::vector<std::unique_ptr<Heightmap>> maskLayers;
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
			maskLayers.push_back(maskGenerator.generateCircleLinearFalloffMask(mapWidth, mapHeight, unaffectedRadiusX, falloffWidth, offsetX, offsetY));
		}
		else if(maskTypeValue == 1) {
			logger << "MaskType: Rectangle\n";
			maskLayers.push_back(maskGenerator.generateRectangleLinearFalloffMask(mapWidth, mapHeight, unaffectedRadiusX, unaffectedRadiusY, falloffWidth, offsetX, offsetY));
		}
		else if (maskTypeValue == 2) {		// TODO: If there already is a linear gradient in the vector, don't add the gradient that goes the opposite way as they will cancel each other out and producte a mask that doesn't do anything (or masks *everything* if it's inverted).
			logger << "MaskType: Linear Gradient";
			RandomUniformInt gradientDirectionValues(0, 3);
			int gradientDirectionIndex = gradientDirectionValues.generate();
			logger << " - DirectionIndex " << gradientDirectionIndex << "\n";
			maskLayers.push_back(maskGenerator.generateLinearGradientMask(mapWidth, mapHeight, gradientDirections[gradientDirectionIndex]));
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
				maskLayers.push_back(absoluteNoiseMapGenerator.generate(frequencyX, frequencyY, offsetX, offsetY));

				RandomBinary invertChance(properties.maskAbsoluteNoiseInversionChance);
				invertLayer = invertChance.generate();
			}
			else {
				logger << "Default\n";
				maskLayers.push_back(noiseMapGenerator.generate(frequencyX, frequencyY, offsetX, offsetY));

				RandomBinary invertChance(properties.maskDefaultNoiseInversionChance);
				invertLayer = invertChance.generate();
			}

			if (invertLayer) {
				logger << "Inverting mask noise layer\n";
				maskLayers.back()->invert();
			}
		}		
	}

	std::unique_ptr<Heightmap> mask = std::move(maskLayers.front());
	maskLayers.erase(maskLayers.begin());
	for (int i = 0; i < maskLayers.size(); i++) {
		mask->add(*(maskLayers.at(i)));
	}

	bool invertMask = binaryChance.generate();
	if (invertMask) {
		logger << "Inverted mask!\n";
		mask->invert();
	}

	return mask;
}

std::unique_ptr<pcb::Heightmap> pcb::RandomHeightmapGenerator::generateFinalMask() const {
	MaskGenerator maskGenerator;
	RandomBinary binaryChance;
	RandomUniformInt layerCountValues(properties.finalMaskAmountOfLayersBounds);
	int amountOfLayers = layerCountValues.generate();
	RandomUniformReal offsetMultiplicationValues(properties.finalMaskOffsetMultiplicationValueBounds);
	RandomUniformReal radiusMultiplicationValues(properties.finalMaskRadiusMultiplicationValueBounds);
	RandomUniformReal falloffMultiplicationValues(properties.finalMaskFalloffMultiplicationValueBounds);
	std::vector<std::unique_ptr<Heightmap>> maskLayers;	
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
			maskLayers.push_back(maskGenerator.generateCircleLinearFalloffMask(mapWidth, mapHeight, unaffectedRadiusX, falloffWidth, offsetX, offsetY));			
		}
		else {
			logger << "MaskType: Rectangle\n";
			maskLayers.push_back(maskGenerator.generateRectangleLinearFalloffMask(mapWidth, mapHeight, unaffectedRadiusX, unaffectedRadiusY, falloffWidth, offsetX, offsetY));
		}

		maskShapeData.emplace_back(offsetX, offsetY, unaffectedRadiusX, unaffectedRadiusY, falloffWidth);
	}

	std::unique_ptr<Heightmap> mask = std::move(maskLayers.front());
	maskLayers.erase(maskLayers.begin());
	for (int i = 0; i < maskLayers.size(); i++) {
		mask->add(*(maskLayers.at(i)));
	}

	bool invertMask = binaryChance.generate();
	if (invertMask) {
		logger << "Inverted mask!\n";
		mask->invert();
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
pcb::LayerData::LayerData(int layerIndex, LayerMode layerMode, const RandomGenerationControlProperties& properties) : layerIndex(layerIndex), layerMode(layerMode) {
	generateNoiseMapGenerationParameters(properties);
}

std::unique_ptr<pcb::LayerData> pcb::LayerData::getRandomAllowedNextNode(const std::vector<std::unique_ptr<LayerData>>& previousLayers, int layerIndex, const RandomGenerationControlProperties& properties) const {
	const std::vector<LayerMode> allowedModes = determineAllowedNextModes(previousLayers);
	int amountOfAvailableModes = allowedModes.size();
	RandomUniformInt layerModeValues(0, amountOfAvailableModes - 1);
	int layerModeIndex = layerModeValues.generate();

	std::unique_ptr<LayerData> nextNode;
	switch (allowedModes.at(layerModeIndex)) {
	case LayerMode::Addition:
		nextNode = std::make_unique<AdditionLayerData>(layerIndex, properties);
		break;
	case LayerMode::Subtraction:
		nextNode = std::make_unique<SubtractionLayerData>(layerIndex, properties);
		break;
	case LayerMode::Mask:
		nextNode = std::make_unique<MaskLayerData>(layerIndex, properties);
		break;
	}

	return nextNode;
}

int pcb::LayerData::getIndex() const {
	return layerIndex;
}

pcb::LayerMode pcb::LayerData::getMode() const {
	return layerMode;
}
pcb::NoiseMapGenerationParameters pcb::LayerData::getNoiseMapGenerationParameters() const {
	return generationParameters;
}

int pcb::LayerData::countAmountOfConsecutiveLayerModesAtEnd(const std::vector<std::unique_ptr<LayerData>>& previousLayers, pcb::LayerMode layerMode) const {
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

void pcb::LayerData::generateNoiseMapGenerationParameters(const RandomGenerationControlProperties& properties) {
	generationParameters.symmetricalFrequency = properties.layerBaseNoiseFrequency - (properties.layerNoiseFrequencyAdditionalLayerModifier * layerIndex);
	generationParameters.noiseOffsetX = generateNoiseOffset(properties);
	generationParameters.noiseOffsetY = generateNoiseOffset(properties);
	generationParameters.invert = false;
	RandomBinary maskChance(properties.applyMaskOnLayerChance);
	generationParameters.applyMask = maskChance.generate();
}

int pcb::LayerData::generateNoiseOffset(const RandomGenerationControlProperties& properties) const {
	RandomUniformInt noiseOffsetValues(properties.noiseOffsetValueBounds);
	return noiseOffsetValues.generate() * noiseOffsetValues.generate();
}

pcb::AdditionLayerData::AdditionLayerData(int layerIndex, const RandomGenerationControlProperties& properties) : LayerData(layerIndex, LayerMode::Addition, properties) {}

std::vector<pcb::LayerMode> pcb::AdditionLayerData::determineAllowedNextModes(const std::vector<std::unique_ptr<LayerData>>& previousLayers) const {
	std::vector<LayerMode> allowedNextModes;
	if (previousLayers.back()->getMode() != LayerMode::Addition) {
		allowedNextModes.push_back(LayerMode::Addition);
	}

	allowedNextModes.push_back(LayerMode::Subtraction);
	allowedNextModes.push_back(LayerMode::Mask);

	return allowedNextModes;
}

pcb::SubtractionLayerData::SubtractionLayerData(int layerIndex, const RandomGenerationControlProperties& properties) : LayerData(layerIndex, LayerMode::Subtraction, properties) {}

std::vector<pcb::LayerMode> pcb::SubtractionLayerData::determineAllowedNextModes(const std::vector<std::unique_ptr<LayerData>>& previousLayers) const {
	std::vector<LayerMode> allowedNextModes;
	allowedNextModes.push_back(LayerMode::Addition);

	if (previousLayers.back()->getMode() == LayerMode::Addition) {
		allowedNextModes.push_back(LayerMode::Mask);
	}	

	return allowedNextModes;
}

pcb::MaskLayerData::MaskLayerData(int layerIndex, const RandomGenerationControlProperties& properties) : LayerData(layerIndex, LayerMode::Mask, properties) {}

std::vector<pcb::LayerMode> pcb::MaskLayerData::determineAllowedNextModes(const std::vector<std::unique_ptr<LayerData>>& previousLayers) const {
	std::vector<LayerMode> allowedNextModes;
	allowedNextModes.push_back(LayerMode::Addition);

	if (previousLayers.back()->getMode() == LayerMode::Addition) {
		allowedNextModes.push_back(LayerMode::Subtraction);
	}	

	return allowedNextModes;
}

pcb::FinalMaskLayerData::FinalMaskLayerData(int layerIndex, const RandomGenerationControlProperties& properties) : LayerData(layerIndex, LayerMode::FinalMask, properties) {}

std::vector<pcb::LayerMode> pcb::FinalMaskLayerData::determineAllowedNextModes(const std::vector<std::unique_ptr<LayerData>>& previousLayers) const {
	return std::vector<LayerMode>();
}
#pragma endregion