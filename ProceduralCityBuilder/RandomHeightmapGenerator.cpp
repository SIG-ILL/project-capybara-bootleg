#include "RandomHeightmapGenerator.hpp"

#include <numbers>
#include <array>
#include <memory>

#include "RandomHeightmapLayerDataGenerator.hpp"

#include "Logger.hpp"

pcb::RandomHeightmapGenerator::RandomHeightmapGenerator(int mapWidth, int mapHeight) : mapWidth(mapWidth), mapHeight(mapHeight), noiseMapGenerator(mapWidth, mapHeight), absoluteNoiseMapGenerator(mapWidth, mapHeight), maskGenerator(mapWidth, mapHeight) {}

// TODO: Rotation of layers (lack of rotation is especially noticeable with 'stretched' noise).
std::unique_ptr<pcb::LayeredHeightmap> pcb::RandomHeightmapGenerator::generate(const RandomHeightmapGenerationData& generationData) const {
	std::unique_ptr<LayeredHeightmap> heightmap = generateLayeredHeightmap(generationData);
	postProcessHeightmap(*heightmap, generationData);

	return heightmap;
}

std::unique_ptr<pcb::LayeredHeightmap> pcb::RandomHeightmapGenerator::generateLayeredHeightmap(const RandomHeightmapGenerationData& generationData) const {
	std::unique_ptr<LayeredHeightmap> heightmap = std::make_unique<LayeredHeightmap>(mapWidth, mapHeight);
	std::unique_ptr<std::vector<std::unique_ptr<HeightmapLayer>>> layers = generateLayers(*(generationData.layerData));

	for (int i = 0; i < layers->size(); i++) {
		heightmap->addLayer(std::move(layers->at(i)));
	}

	return heightmap;
}

std::unique_ptr<std::vector<std::unique_ptr<pcb::HeightmapLayer>>> pcb::RandomHeightmapGenerator::generateLayers(const std::vector<std::unique_ptr<LayerData>>& layerData) const {
	std::unique_ptr<std::vector<std::unique_ptr<HeightmapLayer>>> layers = std::make_unique<std::vector<std::unique_ptr<HeightmapLayer>>>();
	Logger logger;

	for (int i = 0; i < layerData.size(); i++) {
		logger << "Loop index " << i << ", Layer index " << layerData.at(i)->getIndex() << " (these should be the same!)\n";
		switch (layerData.at(i)->getMode()) {
		case LayerMode::Addition:
			logger << "LayerMode: Addition\n";
			break;
		case LayerMode::Subtraction:
			logger << "LayerMode: Subtraction\n";
			break;
		case LayerMode::Mask:
			logger << "LayerMode: Mask\n";
			break;
		case LayerMode::FinalMask:
			logger << "LayerMode: FinalMask\n";
			break;
		}

		if (layerData.at(i)->getMode() == LayerMode::FinalMask) {
			logger << "Generating layer - Final Mask\n";
			layers->push_back(std::make_unique<HeightmapLayer>(generateFinalMask(layerData.at(i)->getFinalMaskGenerationParameters()), LayerMode::Mask));
		}
		else if (layerData.at(i)->getNoiseMapGenerationParameters().isAbsolute) {
			logger << "Generating layer, noise type: Absolute\n";
			layers->push_back(generateNoiseLayer(absoluteNoiseMapGenerator, *(layerData.at(i))));
		}
		else {
			logger << "Generating layer, noise type: Default\n";
			layers->push_back(generateNoiseLayer(noiseMapGenerator, *(layerData.at(i))));
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
		logger << "Raising layer by " << generationParameters.raiseAmountIfMask << " as it is a mask\n";
		layer->heightmap->raise(generationParameters.raiseAmountIfMask);
	}
	else {
		logger << "Scaling layer by " << generationParameters.scalingFactorIfNotMask << " as it is not a mask\n";
		layer->heightmap->scale(generationParameters.scalingFactorIfNotMask);
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
		FinalMaskShapeData layerParameters = finalMaskParameters.layerParameters.at(i);

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

void pcb::RandomHeightmapGenerator::postProcessHeightmap(LayeredHeightmap& heightmap, const RandomHeightmapGenerationData& generationData) const {
	Logger logger;

	if (heightmap.getLowestElevation() > generationData.adjustmentLoweringThresholds.lower && heightmap.getHighestElevation() > generationData.adjustmentLoweringThresholds.upper) {
		logger << "ADJUSTMENT: Lowering heightmap\n";
		heightmap.lower(generationData.adjustmentLoweringValue);
	}

	if (heightmap.getHighestElevation() - heightmap.getLowestElevation() > generationData.adjustmentScaleDownAmplitudeThreshold) {
		logger << "ADJUSTMENT: Scaling down heightmap amplitude\n";
		heightmap.scaleAmplitude(generationData.adjustmentScaleDownAmplitudeValue);
	}
	else if (heightmap.getHighestElevation() - heightmap.getLowestElevation() < generationData.adjustmentScaleUpAmplitudeThreshold) {
		logger << "ADJUSTMENT: Scaling up heightmap amplitude\n";		
		heightmap.scaleAmplitude(generationData.adjustmentScaleUpAmplitudeValue);
	}

	logger << "\n";
}