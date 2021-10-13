#include "RandomHeightmapGenerator.hpp"

#include <numbers>
#include <array>
#include <memory>

#include "RandomUniformInt.hpp"
#include "RandomUniformReal.hpp"
#include "RandomHeightmapLayerDataGenerator.hpp"

#include "Logger.hpp"

pcb::RandomHeightmapGenerator::RandomHeightmapGenerator(int mapWidth, int mapHeight) : RandomHeightmapGenerator(mapWidth, mapHeight, getDefaultControlProperties()) {}

pcb::RandomHeightmapGenerator::RandomHeightmapGenerator(int mapWidth, int mapHeight, const RandomGenerationControlParameters& properties) : mapWidth(mapWidth), mapHeight(mapHeight), properties(properties), noiseMapGenerator(mapWidth, mapHeight), absoluteNoiseMapGenerator(mapWidth, mapHeight), maskGenerator(mapWidth, mapHeight) {}

// TODO: Rotation of layers (lack of rotation is especially noticeable with 'stretched' noise).
std::unique_ptr<pcb::LayeredHeightmap> pcb::RandomHeightmapGenerator::generate() const {
	std::unique_ptr<LayeredHeightmap> heightmap = generateLayeredHeightmap();
	adjustLayeredHeightmap(*heightmap);

	return heightmap;
}

pcb::RandomGenerationControlParameters pcb::RandomHeightmapGenerator::getDefaultControlProperties() const {
	RandomGenerationControlParameters defaultProperties;

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
	defaultProperties.maskInversionChance = 0.5;

	defaultProperties.applyFinalMaskChance = 0.33;
	defaultProperties.finalMaskAmountOfLayersBounds = { 2, 5 };
	defaultProperties.finalMaskOffsetMultiplicationValueBounds = { 0.0, 1.0 };
	defaultProperties.finalMaskRadiusMultiplicationValueBounds = { 0.25, 1.0 };
	defaultProperties.finalMaskFalloffMultiplicationValueBounds = { 0.3, 1.0 };
	defaultProperties.finalMaskCompositeMaskShapesDistanceMultiplierBounds = { 0.5, 1.0 };
	defaultProperties.finalMaskInversionChance = 0.5;

	defaultProperties.adjustmentLoweringThresholds = { 50, 230 };
	defaultProperties.adjustmentLoweringValue = 30;
	defaultProperties.adjustmentScaleDownAmplitudeThreshold = 220;
	defaultProperties.adjustmentScaleDownAmplitudeValueBounds = { 0.45, 0.75 };
	defaultProperties.adjustmentScaleUpAmplitudeThreshold = 100;
	defaultProperties.adjustmentScaleUpAmplitudeValueBounds = { 1.5, 2.2 };

	return defaultProperties;
}

void pcb::RandomHeightmapGenerator::setControlProperties(const RandomGenerationControlParameters& properties) {
	this->properties = properties;
}

std::unique_ptr<pcb::LayeredHeightmap> pcb::RandomHeightmapGenerator::generateLayeredHeightmap() const {
	std::unique_ptr<LayeredHeightmap> heightmap = std::make_unique<LayeredHeightmap>(mapWidth, mapHeight);
	std::unique_ptr<std::vector<std::unique_ptr<HeightmapLayer>>> layers = generateLayers();

	for (int i = 0; i < layers->size(); i++) {
		heightmap->addLayer(std::move(layers->at(i)));
	}

	return heightmap;
}

std::unique_ptr<std::vector<std::unique_ptr<pcb::HeightmapLayer>>> pcb::RandomHeightmapGenerator::generateLayers() const {
	RandomUniformInt layerAmountValues(properties.amountOfLayersBounds);
	int amountOfLayers = layerAmountValues.generate();
	RandomHeightmapLayerDataGenerator layerDataGenerator;
	std::unique_ptr<std::vector<std::unique_ptr<LayerData>>> layerData = layerDataGenerator.generateLayerData(mapWidth, mapHeight, amountOfLayers, properties);
	std::unique_ptr<std::vector<std::unique_ptr<HeightmapLayer>>> layers = std::make_unique<std::vector<std::unique_ptr<HeightmapLayer>>>();
	Logger logger;

	for (int i = 0; i < layerData->size(); i++) {
		logger << "Loop index " << i << ", Layer index " << layerData->at(i)->getIndex() << " (these should be the same!)\n";

		if (layerData->at(i)->getMode() == LayerMode::FinalMask) {
			logger << "Generating layer - Final Mask\n";
			layers->push_back(std::make_unique<HeightmapLayer>(generateFinalMask(layerData->at(i)->getFinalMaskGenerationParameters()), LayerMode::Mask));
		}
		else if (layerData->at(i)->getNoiseMapGenerationParameters().isAbsolute) {
			logger << "Generating layer, noise type: Absolute\n";
			layers->push_back(generateNoiseLayer(absoluteNoiseMapGenerator, *(layerData->at(i))));
		}
		else {
			logger << "Generating layer, noise type: Default\n";
			layers->push_back(generateNoiseLayer(noiseMapGenerator, *(layerData->at(i))));
		}
		
		logger << "\n";
	}

	return layers;
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
		std::unique_ptr<Heightmap> mask = maskGenerator.generateCombinedMask(generationParameters.maskParameters);
		layer->heightmap->mask(*mask);
	}

	return layer;
}

std::unique_ptr<pcb::Heightmap> pcb::RandomHeightmapGenerator::generateFinalMask(const FinalMaskGenerationParameters& finalMaskParameters) const {
	Logger logger;

	std::vector<std::unique_ptr<Heightmap>> maskLayers;
	for (int i = 0; i < finalMaskParameters.layerParameters.size(); i++) {
		MaskShapeData layerParameters = finalMaskParameters.layerParameters.at(i);

		if (layerParameters.maskType == MaskType::Circle) {
			logger << "MaskType: Circle\n";
			maskLayers.push_back(maskGenerator.generateCircleLinearFalloffMask(layerParameters.unaffectedRadiusX, layerParameters.falloffWidth, layerParameters.offsetX, layerParameters.offsetY));
		}
		else if(layerParameters.maskType == MaskType::Rectangle) {
			logger << "MaskType: Rectangle\n";
			maskLayers.push_back(maskGenerator.generateRectangleLinearFalloffMask(layerParameters.unaffectedRadiusX, layerParameters.unaffectedRadiusY, layerParameters.falloffWidth, layerParameters.offsetX, layerParameters.offsetY));
		}
	}

	std::unique_ptr<Heightmap> mask = std::move(maskLayers.front());
	maskLayers.erase(maskLayers.begin());
	for (int i = 0; i < maskLayers.size(); i++) {
		mask->add(*(maskLayers.at(i)));
	}

	if (finalMaskParameters.invert) {
		logger << "Inverting final mask!\n";
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