#include "LayeredTerrain.hpp"

pcb::LayeredTerrain::LayeredTerrain(const pcb::LayeredHeightmap& layeredHeightmap, double scale) : Terrain(layeredHeightmap, scale), layers() {
	for (const pcb::HeightmapLayer& heightmapLayer : layeredHeightmap.getLayers()) {
		layers.emplace_back(Terrain(heightmapLayer.heightmap, scale));

		if (heightmapLayer.mode == pcb::LayerMode::Addition) {
			layers.back().setHeightBasedColorGradient(0, 0, 0, 0, 1, 0, true);
		}
		else if (heightmapLayer.mode == pcb::LayerMode::Subtraction) {
			layers.back().setHeightBasedColorGradient(0, 0, 0, 1, 0, 0, true);
		}
	}
}

std::vector<pcb::Terrain> pcb::LayeredTerrain::getLayers() const {
	return layers;
}

int pcb::LayeredTerrain::getLayerCount() const {
	return layers.size();
}