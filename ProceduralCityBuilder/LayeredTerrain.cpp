#include "LayeredTerrain.hpp"

#include <thread>

pcb::LayeredTerrain::LayeredTerrain(const pcb::LayeredHeightmap& layeredHeightmap, double scale) : Terrain(layeredHeightmap, scale), layers(layeredHeightmap.getLayerCount()) {
	std::shared_ptr<std::vector<std::shared_ptr<HeightmapLayer>>> heightmapLayers = layeredHeightmap.getLayers();
	std::vector<std::thread> layerGenerationThreads;
	for (int i = 0; i < heightmapLayers->size(); i++) {
		std::shared_ptr heightmapLayer = heightmapLayers->at(i);
		layerGenerationThreads.emplace_back(&LayeredTerrain::generateLayer, this, i, *heightmapLayer, scale);
	}

	for (int i = 0; i < layerGenerationThreads.size(); i++) {
		layerGenerationThreads.at(i).join();
	}
}

void pcb::LayeredTerrain::generateLayer(int index, const HeightmapLayer& heightmapLayer, double scale) {
	GLfloat maxRed = 0.0f;
	GLfloat maxGreen = 0.0f;
	GLfloat maxBlue = 0.0f;

	if (heightmapLayer.mode == pcb::LayerMode::Addition) {
		maxGreen = 1.0f;
	}
	else if (heightmapLayer.mode == pcb::LayerMode::Subtraction) {
		maxRed = 1.0f;
	}
	else if (heightmapLayer.mode == LayerMode::Mask) {
		maxBlue = 1.0f;
	}

	layers.at(index) = std::make_shared<Terrain>(*(heightmapLayer.heightmap), scale, 0.0f, 0.0f, 0.0f, maxRed, maxGreen, maxBlue, true);
}

std::vector<std::shared_ptr<pcb::Terrain>> pcb::LayeredTerrain::getLayers() const {
	return layers;
}

int pcb::LayeredTerrain::getLayerCount() const {
	return layers.size();
}