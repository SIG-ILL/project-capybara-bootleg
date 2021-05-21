#include "LayeredHeightmapGenerator.hpp"

#include <cmath>

pcb::LayeredHeightmapGenerator::LayeredHeightmapGenerator(int mapWidth, int mapHeight) : mapWidth(mapWidth), mapHeight(mapHeight), noiseGenerator() {}

pcb::LayeredHeightmap* pcb::LayeredHeightmapGenerator::generateNew() const {
	pcb::Heightmap heightmapLayer = generateHeightmap(0.025, 0.85, 1);
	pcb::LayeredHeightmap* heightmap = new pcb::LayeredHeightmap(mapWidth, mapHeight, heightmapLayer);
	heightmap->addLayer(generateHeightmap(0.05, 0.25, 0.4), LayerMode::Addition);
	heightmap->addLayer(generateHeightmap(0.1, 0.1, 0.3), LayerMode::Addition);
	heightmap->addLayer(generateHeightmap(0.25, 0.15, 0.2), LayerMode::Addition);
	heightmap->addLayer(generateHeightmap(0.05, 0.15, 0.2), LayerMode::Subtraction);
	heightmap->addLayer(generateHeightmap(0.1, 0.3, 0.1), LayerMode::Subtraction);
	heightmap->addLayer(generateHeightmap(0.25, 0.55, 0.4), LayerMode::Subtraction);

	return heightmap;
}

pcb::Heightmap pcb::LayeredHeightmapGenerator::generateHeightmap(double noiseModifier, double multiplier, double maxValueFactor) const {
	if (maxValueFactor > 1.0) {
		maxValueFactor = 1.0;
	}
	if (maxValueFactor < 0.0) {
		maxValueFactor = 0.0;
	}

	unsigned char* noiseMap = new unsigned char[mapWidth * mapHeight];
	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			double noiseInputX = x * noiseModifier;
			double noiseInputY = y * noiseModifier;
			noiseMap[(y * mapWidth) + x] = static_cast<unsigned char>(multiplier * std::round(maxValueFactor * generateValueForCoordinates(noiseInputX, noiseInputY)));
		}
	}
	pcb::Heightmap heightmap(mapWidth, mapHeight, noiseMap);
	delete[] noiseMap;

	return heightmap;
}

double pcb::LayeredHeightmapGenerator::generateValueForCoordinates(double x, double y) const {
	return 127.5 * (1 + noiseGenerator.getValueForCoordinates(x, y));
}