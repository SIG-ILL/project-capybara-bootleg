#include "TerrainGenerator.hpp"

#include "HeightmapGenerator.hpp"

pcb::TerrainGenerator::TerrainGenerator() : heightmap(nullptr) {}

pcb::TerrainGenerator::~TerrainGenerator() {
	delete heightmap;
}

pcb::Terrain* pcb::TerrainGenerator::generateNew() {
	int mapWidth = 256;
	int mapHeight = 256;

	pcb::HeightmapGenerator heightmapGenerator(mapWidth, mapHeight);
	delete heightmap;
	heightmap = heightmapGenerator.generateNew();

	pcb::Terrain* terrain = new pcb::Terrain(heightmap, 1.0 / mapWidth);

	return terrain;
}

pcb::Image* pcb::TerrainGenerator::getHeightmap24BitImageNew() {
	return heightmap->to24BitImageNew();
}