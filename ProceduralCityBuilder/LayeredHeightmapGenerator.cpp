#include "LayeredHeightmapGenerator.hpp"

#include <cmath>
#include <random>

#include "MaskGenerator.hpp"
#include "RandomHeightmapGenerator.hpp"

pcb::LayeredHeightmapGenerator::LayeredHeightmapGenerator(int mapWidth, int mapHeight) : mapWidth(mapWidth), mapHeight(mapHeight), noiseGenerator() {}

pcb::LayeredHeightmap* pcb::LayeredHeightmapGenerator::generateNew() const {
	std::vector<Heightmap> heightmapLayers;
	MaskGenerator maskGenerator;
	heightmapLayers.emplace_back(maskGenerator.generateCircleLinearFalloffMask(mapWidth, mapHeight, (mapWidth/256.0f) * 30, (mapWidth / 256.0f) * 98, 0, 0));

	heightmapLayers.emplace_back(generateHeightmap(128, 128, 0, 0));
	heightmapLayers.back().lowerToLevel(175);
	heightmapLayers.back().scale(0.5);
	LayeredHeightmap* heightmap = new LayeredHeightmap(heightmapLayers.back());

	heightmapLayers.emplace_back(generateHeightmap(1024, 128, 8, 0));
	heightmapLayers.back().scale(0.5);
	heightmapLayers.back().scaleAmplitude(2);	
	heightmapLayers.back().mask(heightmapLayers.at(0));
	heightmap->addLayer(heightmapLayers.back(), LayerMode::Addition);

	heightmapLayers.emplace_back(generateHeightmap(32, 32, 0, 8));
	heightmapLayers.back().scale(0.0625);
	heightmap->addLayer(heightmapLayers.back(), LayerMode::Addition);

	heightmapLayers.emplace_back(generateHeightmap(16, 16, 8, 8));
	heightmapLayers.back().scale(0.03125);
	heightmap->addLayer(heightmapLayers.back(), LayerMode::Addition);

	heightmap->scale(1.5);
	heightmap->addLayer(heightmapLayers.at(0), LayerMode::Mask);
	heightmapLayers.emplace_back(maskGenerator.generateRectangleLinearFalloffMask(mapWidth, mapHeight, (mapWidth / 256.0f) * 15, (mapHeight / 256.0f) * 15, (mapWidth / 256.0f) * 64, 0, 0));
	heightmapLayers.back().invert();
	heightmap->addLayer(heightmapLayers.back(), LayerMode::Mask);

	heightmapLayers.emplace_back(generateHeightmap(16, 16, 0, 0));
	heightmapLayers.back().mask(*heightmap);
	heightmapLayers.at(0).invert();
	heightmapLayers.back().mask(heightmapLayers.at(0));
	heightmap->addLayer(heightmapLayers.back(), LayerMode::Addition);
	heightmapLayers.at(3).scale(10);
	heightmap->addLayer(heightmapLayers.at(3), LayerMode::Mask);

	return heightmap;
}

pcb::LayeredHeightmap* pcb::LayeredHeightmapGenerator::generateNewRandom() const {
	RandomHeightmapGenerator generator(mapWidth, mapHeight);
	return generator.generateNew();
}

pcb::Heightmap pcb::LayeredHeightmapGenerator::generateHeightmap(double noiseSamplingFrequencyX, double noiseSamplingFrequencyY, double xOffset, double yOffset) const {
	unsigned char* noiseMap = new unsigned char[mapWidth * mapHeight];
	const double samplingFrequencyToSamplingDistanceModifierX = 1.0 / noiseSamplingFrequencyX;
	const double samplingFrequencyToSamplingDistanceModifierY = 1.0 / noiseSamplingFrequencyY;

	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			double noiseInputX = xOffset + (x * samplingFrequencyToSamplingDistanceModifierX);
			double noiseInputY = yOffset + (y * samplingFrequencyToSamplingDistanceModifierY);
			noiseMap[(y * mapWidth) + x] = static_cast<unsigned char>(generateElevationForNoiseCoordinates(noiseInputX, noiseInputY));
		}
	}

	pcb::Heightmap heightmap(mapWidth, mapHeight, noiseMap);
	delete[] noiseMap;

	return heightmap;
}

double pcb::LayeredHeightmapGenerator::generateElevationForNoiseCoordinates(double x, double y) const {
	return std::round(127.5 * (1 + noiseGenerator.getValueForCoordinate(x, y)));
}