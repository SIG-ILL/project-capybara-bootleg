#include "LayeredHeightmap.hpp"

pcb::LayeredHeightmap::LayeredHeightmap(int width, int height) : Heightmap(width, height), layers() {}

pcb::LayeredHeightmap::LayeredHeightmap(int width, int height, unsigned char* elevationValues) : Heightmap(width, height, elevationValues), layers(1, pcb::HeightmapLayer{pcb::Heightmap(width, height, elevationValues), pcb::LayerMode::Addition}) {}

pcb::LayeredHeightmap::LayeredHeightmap(const pcb::Heightmap& baseHeightmap) : Heightmap(baseHeightmap), layers(1, pcb::HeightmapLayer{ baseHeightmap, pcb::LayerMode::Addition }) {}

pcb::LayeredHeightmap& pcb::LayeredHeightmap::operator+=(const pcb::Heightmap& other) {
	addLayer(other, pcb::LayerMode::Addition);
	return *this;
}

pcb::LayeredHeightmap& pcb::LayeredHeightmap::operator+=(const pcb::LayeredHeightmap& other) {
	addLayer(other, pcb::LayerMode::Addition);
	return *this;
}

pcb::LayeredHeightmap& pcb::LayeredHeightmap::operator-=(const pcb::Heightmap& other) {
	addLayer(other, pcb::LayerMode::Subtraction);
	return *this;
}

pcb::LayeredHeightmap& pcb::LayeredHeightmap::operator-=(const pcb::LayeredHeightmap& other) {
	addLayer(other, pcb::LayerMode::Subtraction);
	return *this;
}

void pcb::LayeredHeightmap::addLayer(const pcb::Heightmap& heightmap, pcb::LayerMode mode) {
	layers.push_back(HeightmapLayer{ heightmap, mode });

	if (mode == LayerMode::Addition) {
		add(heightmap);
	}
	else if (mode == LayerMode::Subtraction) {
		subtract(heightmap);
	}
	else if (mode == LayerMode::Mask) {
		mask(heightmap);
	}
}

int pcb::LayeredHeightmap::getLayerCount() const {
	return layers.size();
}

std::vector<pcb::HeightmapLayer> pcb::LayeredHeightmap::getLayers() const {
	return layers;
}