#include "LayeredTerrainGenerator.hpp"

#include "LayeredHeightmapGenerator.hpp"
#include "Logger.hpp"

pcb::LayeredTerrainGenerator::LayeredTerrainGenerator(int mapWidth, int mapHeight, double scale) : mapWidth(mapWidth), mapHeight(mapHeight), scale(scale) {}

std::unique_ptr<pcb::LayeredTerrain> pcb::LayeredTerrainGenerator::generate() {
	LayeredHeightmapGenerator heightmapGenerator(mapWidth, mapHeight);
	heightmap = heightmapGenerator.generate();

	double terrainScale = scale * (1.0f / 255);		// 255 is current heightmap maximum elevation.
	return std::make_unique<LayeredTerrain>(*heightmap, terrainScale);
}

std::unique_ptr<pcb::LayeredTerrain> pcb::LayeredTerrainGenerator::generateRandom() {
	Logger logger;
	logger << "Generating random heightmap...\n";

	LayeredHeightmapGenerator heightmapGenerator(mapWidth, mapHeight);
	heightmap = heightmapGenerator.generateRandom();

	logger << "Random heightmap generation done!\nCreating layered terrain... ";

	double terrainScale = scale * (1.0f / 255);		// 255 is current heightmap maximum elevation.
	return std::make_unique<LayeredTerrain>(*heightmap, terrainScale);
}

std::unique_ptr<pcb::HeightmapImage> pcb::LayeredTerrainGenerator::getHeightmap24BitImage() const {
	std::shared_ptr<std::vector<std::shared_ptr<HeightmapLayer>>> heightmapLayers = heightmap->getLayers();
	std::vector<std::shared_ptr<Image>> layerImages;
	for (int i = 0; i < heightmapLayers->size(); i++) {
		layerImages.push_back(heightmapLayers->at(i)->heightmap->to24BitImage());
	}

	return std::make_unique<HeightmapImage>(heightmap->to24BitImage(), layerImages);
}