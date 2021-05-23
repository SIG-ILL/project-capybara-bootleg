#include "TerrainGenerator.hpp"

#include "HeightmapGenerator.hpp"

pcb::TerrainGenerator::TerrainGenerator(int mapWidth, int mapHeight, double scale) : mapWidth(mapWidth), mapHeight(mapHeight), scale(scale), heightmap(nullptr) {}

pcb::TerrainGenerator::~TerrainGenerator() {
	delete heightmap;
}

pcb::Terrain* pcb::TerrainGenerator::generateNew() {
	pcb::HeightmapGenerator heightmapGenerator(mapWidth, mapHeight);
	delete heightmap;
	heightmap = heightmapGenerator.generateNew();

	pcb::Terrain* terrain = new pcb::Terrain(*heightmap, scale / mapWidth);

	return terrain;
}

pcb::Image* pcb::TerrainGenerator::getHeightmap24BitImageNew() const {
	return heightmap->to24BitImageNew();
}