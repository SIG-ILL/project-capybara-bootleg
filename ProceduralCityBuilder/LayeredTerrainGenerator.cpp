#include "LayeredTerrainGenerator.hpp"

#include "LayeredHeightmapGenerator.hpp"

pcb::LayeredTerrainGenerator::LayeredTerrainGenerator(int mapWidth, int mapHeight, double scale) : mapWidth(mapWidth), mapHeight(mapHeight), scale(scale) {}

std::unique_ptr<pcb::LayeredTerrain> pcb::LayeredTerrainGenerator::generate() {
	LayeredHeightmapGenerator heightmapGenerator(mapWidth, mapHeight);
	heightmap = std::make_unique<LayeredHeightmap>(heightmapGenerator.generate());

	double terrainScale = scale * (1.0f / 255);		// 255 is current heightmap maximum elevation.
	return std::make_unique<LayeredTerrain>(*heightmap, terrainScale);
}

std::unique_ptr<pcb::LayeredTerrain> pcb::LayeredTerrainGenerator::generateRandom() {
	LayeredHeightmapGenerator heightmapGenerator(mapWidth, mapHeight);
	heightmap = heightmapGenerator.generateRandom();

	double terrainScale = scale * (1.0f / 255);		// 255 is current heightmap maximum elevation.
	return std::make_unique<LayeredTerrain>(*heightmap, terrainScale);
}

std::unique_ptr<pcb::HeightmapImage> pcb::LayeredTerrainGenerator::getHeightmap24BitImage() const {
	std::vector<std::shared_ptr<HeightmapLayer>> heightmapLayers = heightmap->getLayers();
	std::vector<Image> layerImages;
	for (int i = 0; i < heightmapLayers.size(); i++) {
		layerImages.emplace_back(heightmapLayers.at(i)->heightmap->to24BitImage());
	}

	return std::make_unique<HeightmapImage>(heightmap->to24BitImage(), layerImages);
}