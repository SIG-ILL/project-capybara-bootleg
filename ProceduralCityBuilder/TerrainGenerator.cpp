#include "TerrainGenerator.hpp"

#include "HeightmapGenerator.hpp"

pcb::TerrainGenerator::TerrainGenerator(int mapWidth, int mapHeight, double scale) : LayeredTerrainGenerator(mapWidth, mapHeight, scale) {}

std::unique_ptr<pcb::LayeredTerrain> pcb::TerrainGenerator::generate() {
	HeightmapGenerator heightmapGenerator(mapWidth, mapHeight);
	heightmap = std::make_unique<LayeredHeightmap>(heightmapGenerator.generate());

	double terrainScale = scale * (1.0f / 255);		// 255 is current heightmap maximum elevation.
	return std::make_unique<LayeredTerrain>(*heightmap, terrainScale);
}

std::unique_ptr<pcb::HeightmapImage> pcb::TerrainGenerator::getHeightmap24BitImage() const {
	return std::make_unique<HeightmapImage>(heightmap->to24BitImage(), std::vector<Image> { heightmap->to24BitImage() });
}