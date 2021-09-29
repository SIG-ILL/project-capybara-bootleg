#include "LayeredTerrain.hpp"

pcb::LayeredTerrain::LayeredTerrain(const pcb::LayeredHeightmap& layeredHeightmap, double scale) : Terrain(layeredHeightmap, scale), layers() {
	std::shared_ptr<std::vector<std::shared_ptr<HeightmapLayer>>> heightmapLayers = layeredHeightmap.getLayers();
	for (int i = 0; i < heightmapLayers->size(); i++) {
		layers.push_back(std::make_shared<Terrain>(*(heightmapLayers->at(i)->heightmap), scale));

		if (heightmapLayers->at(i)->mode == pcb::LayerMode::Addition) {
			layers.back()->setHeightBasedColorGradient(0, 0, 0, 0, 1, 0, true);
		}
		else if (heightmapLayers->at(i)->mode == pcb::LayerMode::Subtraction) {
			layers.back()->setHeightBasedColorGradient(0, 0, 0, 1, 0, 0, true);
		}
		else if (heightmapLayers->at(i)->mode == LayerMode::Mask) {
			layers.back()->setHeightBasedColorGradient(0, 0, 0, 0, 0, 1, true);
		}
	}
}

std::vector<std::shared_ptr<pcb::Terrain>> pcb::LayeredTerrain::getLayers() const {
	return layers;
}

int pcb::LayeredTerrain::getLayerCount() const {
	return layers.size();
}