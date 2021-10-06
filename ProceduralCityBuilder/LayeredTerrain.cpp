#include "LayeredTerrain.hpp"

pcb::LayeredTerrain::LayeredTerrain(const pcb::LayeredHeightmap& layeredHeightmap, double scale) : Terrain(layeredHeightmap, scale), layers() {
	std::shared_ptr<std::vector<std::shared_ptr<HeightmapLayer>>> heightmapLayers = layeredHeightmap.getLayers();
	for (int i = 0; i < heightmapLayers->size(); i++) {
		GLfloat maxRed = 0.0f;
		GLfloat maxGreen = 0.0f;
		GLfloat maxBlue = 0.0f;

		if (heightmapLayers->at(i)->mode == pcb::LayerMode::Addition) {
			maxGreen = 1.0f;
		}
		else if (heightmapLayers->at(i)->mode == pcb::LayerMode::Subtraction) {
			maxRed = 1.0f;
		}
		else if (heightmapLayers->at(i)->mode == LayerMode::Mask) {
			maxBlue = 1.0f;
		}

		layers.push_back(std::make_shared<Terrain>(*(heightmapLayers->at(i)->heightmap), scale, 0.0f, 0.0f, 0.0f, maxRed, maxGreen, maxBlue, true));
	}
}

std::vector<std::shared_ptr<pcb::Terrain>> pcb::LayeredTerrain::getLayers() const {
	return layers;
}

int pcb::LayeredTerrain::getLayerCount() const {
	return layers.size();
}