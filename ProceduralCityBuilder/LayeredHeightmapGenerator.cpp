#include "LayeredHeightmapGenerator.hpp"

#include <cmath>
#include <random>

#include "MaskGenerator.hpp"
#include "RandomHeightmapGenerator.hpp"
#include "RandomUniformInt.hpp"
#include "RandomUniformReal.hpp"

pcb::LayeredHeightmapGenerator::LayeredHeightmapGenerator(int mapWidth, int mapHeight) : mapWidth(mapWidth), mapHeight(mapHeight), noiseGenerator() {}

std::unique_ptr<pcb::LayeredHeightmap> pcb::LayeredHeightmapGenerator::generate() const {
	std::vector<std::shared_ptr<Heightmap>> heightmapLayers;
	MaskGenerator maskGenerator(mapWidth, mapHeight);
	heightmapLayers.push_back(maskGenerator.generateCircleLinearFalloffMask((mapWidth/256.0f) * 30, (mapWidth / 256.0f) * 98, 0, 0));

	heightmapLayers.push_back(generateHeightmap(128, 128, 0, 0));
	heightmapLayers.back()->lowerToLevel(175);
	heightmapLayers.back()->scale(0.5);
	std::unique_ptr<LayeredHeightmap> heightmap = std::make_unique<LayeredHeightmap>(*(heightmapLayers.back()));

	heightmapLayers.push_back(generateHeightmap(1024, 128, 8, 0));
	heightmapLayers.back()->scale(0.5);
	heightmapLayers.back()->scaleAmplitude(2);	
	heightmapLayers.back()->mask(*(heightmapLayers.at(0)));
	heightmap->addLayer(heightmapLayers.back(), LayerMode::Addition);

	heightmapLayers.push_back(generateHeightmap(32, 32, 0, 8));
	heightmapLayers.back()->scale(0.0625);
	heightmap->addLayer(heightmapLayers.back(), LayerMode::Addition);

	heightmapLayers.push_back(generateHeightmap(16, 16, 8, 8));
	heightmapLayers.back()->scale(0.03125);
	heightmap->addLayer(heightmapLayers.back(), LayerMode::Addition);

	heightmap->scale(1.5);
	heightmap->addLayer(heightmapLayers.at(0), LayerMode::Mask);
	heightmapLayers.push_back(maskGenerator.generateRectangleLinearFalloffMask((mapWidth / 256.0f) * 15, (mapHeight / 256.0f) * 15, (mapWidth / 256.0f) * 64, 0, 0));
	heightmapLayers.back()->invert();
	heightmap->addLayer(heightmapLayers.back(), LayerMode::Mask);

	heightmapLayers.push_back(generateHeightmap(16, 16, 0, 0));
	heightmapLayers.back()->mask(*heightmap);
	heightmapLayers.at(0)->invert();
	heightmapLayers.back()->mask(*(heightmapLayers.at(0)));
	heightmap->addLayer(heightmapLayers.back(), LayerMode::Addition);
	heightmapLayers.at(3)->scale(10);
	heightmap->addLayer(heightmapLayers.at(3), LayerMode::Mask);

	return heightmap;
}

std::unique_ptr<pcb::LayeredHeightmap> pcb::LayeredHeightmapGenerator::generateRandom() const {
	RandomHeightmapGenerator generator(mapWidth, mapHeight);
	RandomHeightmapGenerationParameters parameters = generator.getDefaultGenerationParameters();
	RandomGenerationControlParameters randomGenerationControlParameters = generator.getDefaultRandomGenerationControlParameters();

	RandomUniformInt layerAmountValues(randomGenerationControlParameters.amountOfLayersBounds);
	RandomUniformReal scaleDownAmplitudeFactorValues(randomGenerationControlParameters.adjustmentScaleDownAmplitudeValueBounds);
	RandomUniformReal scaleUpAmplitudeFactorValues(randomGenerationControlParameters.adjustmentScaleUpAmplitudeValueBounds);
	parameters.globalParameters.amountOfLayers = layerAmountValues.generate();
	parameters.globalParameters.adjustmentScaleDownAmplitudeValue = scaleDownAmplitudeFactorValues.generate();
	parameters.globalParameters.adjustmentScaleUpAmplitudeValue = scaleUpAmplitudeFactorValues.generate();
	parameters.layerParameters.layerBaseNoiseFrequency = 256;
	parameters.layerParameters.layerNoiseFrequencyAdditionalLayerModifier = 20;
	generator.setGenerationParameters(parameters);

	RandomHeightmapLayerDataGenerator layerDataGenerator;
	std::unique_ptr<std::vector<std::unique_ptr<LayerData>>> layerData = layerDataGenerator.generateLayerData(mapWidth, mapHeight, randomGenerationControlParameters, parameters);
	return generator.generate(*layerData);
}

std::unique_ptr<pcb::Heightmap> pcb::LayeredHeightmapGenerator::generateHeightmap(double noiseSamplingFrequencyX, double noiseSamplingFrequencyY, double xOffset, double yOffset) const {
	std::shared_ptr<std::vector<unsigned char>> noiseMap = std::make_shared<std::vector<unsigned char>>();
	noiseMap->reserve(mapWidth * mapHeight);
	const double samplingFrequencyToSamplingDistanceModifierX = 1.0 / noiseSamplingFrequencyX;
	const double samplingFrequencyToSamplingDistanceModifierY = 1.0 / noiseSamplingFrequencyY;

	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			double noiseInputX = xOffset + (x * samplingFrequencyToSamplingDistanceModifierX);
			double noiseInputY = yOffset + (y * samplingFrequencyToSamplingDistanceModifierY);
			noiseMap->push_back(static_cast<unsigned char>(generateElevationForNoiseCoordinates(noiseInputX, noiseInputY)));
		}
	}

	return std::make_unique<Heightmap>(mapWidth, mapHeight, noiseMap);
}

double pcb::LayeredHeightmapGenerator::generateElevationForNoiseCoordinates(double x, double y) const {
	return std::round(127.5 * (1 + noiseGenerator.getValueForCoordinate(x, y)));
}