#include "LayeredHeightmapGenerator.hpp"

#include <cmath>

pcb::LayeredHeightmapGenerator::LayeredHeightmapGenerator(int mapWidth, int mapHeight) : mapWidth(mapWidth), mapHeight(mapHeight), noiseGenerator() {}

pcb::Heightmap generateMask(int width, int height, int unaffectedCircleRadiusInPixels, int fadingRingWidthInPixels) {
	unsigned char* maskData = new unsigned char[width * height];

	const float centerX = width / 2.0f;
	const float centerY = height / 2.0f;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			unsigned char value = 255;
			float horizontalDistance = std::abs(x - centerX);
			float verticalDistance = std::abs(y - centerY);
			float distanceToCenter = std::floor(std::sqrt((horizontalDistance * horizontalDistance) + (verticalDistance * verticalDistance)));
			if (distanceToCenter > unaffectedCircleRadiusInPixels) {
				float progressIntoFadingRing = std::min(((distanceToCenter - unaffectedCircleRadiusInPixels) / fadingRingWidthInPixels), 1.0f);
				value = static_cast<unsigned char>(255 - (progressIntoFadingRing * 255));
			}
			maskData[(y * width) + x] = value;
		}
	}

	pcb::Heightmap mask(width, height, maskData);
	delete[] maskData;

	return mask;
}

pcb::LayeredHeightmap* pcb::LayeredHeightmapGenerator::generateNew() const {
	std::vector<Heightmap> heightmapLayers;
	heightmapLayers.emplace_back(generateMask(mapWidth, mapHeight, 30, 98));

	heightmapLayers.emplace_back(generateHeightmap(128, 128, 0, 0));
	heightmapLayers.back().lowerToLevel(175);
	heightmapLayers.back().scale(0.5);
	pcb::LayeredHeightmap* heightmap = new pcb::LayeredHeightmap(heightmapLayers.back());

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
	heightmap->mask(heightmapLayers.at(0));

	return heightmap;
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
	return std::round(127.5 * (1 + noiseGenerator.getValueForCoordinates(x, y)));
}