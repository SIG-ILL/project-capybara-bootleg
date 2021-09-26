#include "HeightmapGenerator.hpp"

#include <cmath>

pcb::HeightmapGenerator::HeightmapGenerator(int mapWidth, int mapHeight) : mapWidth(mapWidth), mapHeight(mapHeight), noiseGenerator() {}

pcb::Heightmap pcb::HeightmapGenerator::generate() const {
	std::vector<unsigned char> noiseMap(mapWidth * mapHeight, 0);

	generateAndAddNoiseMap(noiseMap, 0.025, 0.85, 1);
	generateAndAddNoiseMap(noiseMap, 0.05, 0.25, 0.4);
	generateAndAddNoiseMap(noiseMap, 0.1, 0.1, 0.3);
	generateAndAddNoiseMap(noiseMap, 0.25, 0.15, 0.2);
	generateAndSubtractNoiseMap(noiseMap, 0.05, 0.15, 0.2);
	generateAndSubtractNoiseMap(noiseMap, 0.1, 0.3, 0.1);
	generateAndSubtractNoiseMap(noiseMap, 0.25, 0.55, 0.4);

	Heightmap heightmap(mapWidth, mapHeight, noiseMap);
	return heightmap;
}

void pcb::HeightmapGenerator::generateAndAddNoiseMap(std::vector<unsigned char>& noiseMap, double noiseModifier, double multiplier, double maxValueFactor) const {
	if (maxValueFactor > 1.0) {
		maxValueFactor = 1.0;
	}
	if (maxValueFactor < 0.0) {
		maxValueFactor = 0.0;
	}

	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			double noiseInputX = x * noiseModifier;
			double noiseInputY = y * noiseModifier;
			noiseMap.at((y * mapWidth) + x) = static_cast<unsigned char>(std::min(noiseMap.at((y * mapWidth) + x) + static_cast<unsigned char>(multiplier * std::round(maxValueFactor * generateElevationForNoiseCoordinates(noiseInputX, noiseInputY))), 255));
		}
	}
}

void pcb::HeightmapGenerator::generateAndSubtractNoiseMap(std::vector<unsigned char>& noiseMap, double noiseModifier, double multiplier, double maxValueFactor) const {
	if (maxValueFactor > 1.0) {
		maxValueFactor = 1.0;
	}
	if (maxValueFactor < 0.0) {
		maxValueFactor = 0.0;
	}

	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			double noiseInputX = x * noiseModifier;
			double noiseInputY = y * noiseModifier;
			noiseMap.at((y * mapWidth) + x) = static_cast<unsigned char>(std::max(0, noiseMap.at((y * mapWidth) + x) - static_cast<unsigned char>(multiplier * std::round(maxValueFactor * generateElevationForNoiseCoordinates(noiseInputX, noiseInputY)))));
		}
	}
}

double pcb::HeightmapGenerator::generateElevationForNoiseCoordinates(double x, double y) const {
	return 127.5 * (1 + noiseGenerator.getValueForCoordinate(x, y));
}