#include "LayeredTerrainGenerator.hpp"

#include "LayeredHeightmapGenerator.hpp"

pcb::LayeredTerrainGenerator::LayeredTerrainGenerator(int mapWidth, int mapHeight, double scale) : mapWidth(mapWidth), mapHeight(mapHeight), scale(scale), heightmap(nullptr) {}

pcb::LayeredTerrainGenerator::~LayeredTerrainGenerator() {
	delete heightmap;
}

pcb::LayeredTerrain* pcb::LayeredTerrainGenerator::generateNew() {
	pcb::LayeredHeightmapGenerator heightmapGenerator(mapWidth, mapHeight);
	delete heightmap;
	heightmap = heightmapGenerator.generateNew();

	pcb::LayeredTerrain* terrain = new pcb::LayeredTerrain(*heightmap, (1.0f / 255) /*scale / mapWidth*/);

	return terrain;
}

pcb::LayeredTerrain* pcb::LayeredTerrainGenerator::generateNewRandom() {
	pcb::LayeredHeightmapGenerator heightmapGenerator(mapWidth, mapHeight);
	delete heightmap;
	heightmap = heightmapGenerator.generateNewRandom();

	pcb::LayeredTerrain* terrain = new pcb::LayeredTerrain(*heightmap, (1.0f / 255) /*scale / mapWidth*/);

	return terrain;
}

pcb::Image* pcb::LayeredTerrainGenerator::getHeightmap24BitImageNew() const {
	return heightmap->to24BitImageNew();
}

std::vector<pcb::Image*> pcb::LayeredTerrainGenerator::getHeightmapLayers24BitImagesNew() const {
	std::vector<pcb::Image*> images;
	for (const pcb::HeightmapLayer& heightmapLayer : heightmap->getLayers()) {
		images.push_back(heightmapLayer.heightmap.to24BitImageNew());
	}

	return images;
}