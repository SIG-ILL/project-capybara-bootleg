#include "TerrainGenerator.hpp"

#include "HeightMapGenerator.hpp"

pcb::TerrainGenerator::TerrainGenerator() : heightMap(nullptr) {}

pcb::TerrainGenerator::~TerrainGenerator() {
	delete heightMap;
}

pcb::Terrain* pcb::TerrainGenerator::generateNew() {
	int mapWidth = 256;
	int mapHeight = 256;

	pcb::HeightMapGenerator heightMapGenerator(mapWidth, mapHeight);
	delete heightMap;
	heightMap = heightMapGenerator.generateNew();

	pcb::Terrain* terrain = new pcb::Terrain(heightMap, 1.0 / mapWidth);

	return terrain;
}

pcb::Image* pcb::TerrainGenerator::getHeightMap24BitImageNew() {
	return heightMap->to24BitImageNew();
}