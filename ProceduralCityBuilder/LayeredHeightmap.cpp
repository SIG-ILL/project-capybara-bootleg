#include "LayeredHeightmap.hpp"

pcb::LayeredHeightmap::LayeredHeightmap(int width, int height, unsigned char* elevationValues) : Heightmap(width, height, elevationValues), layers(1, pcb::HeightmapLayer{pcb::Heightmap(width, height, elevationValues), pcb::LayerMode::Addition}) {}

pcb::LayeredHeightmap::LayeredHeightmap(int width, int height, pcb::Heightmap baseHeightmap) : Heightmap(baseHeightmap), layers(1, pcb::HeightmapLayer{ baseHeightmap, pcb::LayerMode::Addition }) {}

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

int pcb::LayeredHeightmap::getWidth() const {
	return layers.at(0).heightmap.getWidth();
}

int pcb::LayeredHeightmap::getHeight() const {
	return layers.at(0).heightmap.getHeight();
}

void pcb::LayeredHeightmap::addLayer(pcb::Heightmap heightmap, pcb::LayerMode mode) {
	layers.push_back(pcb::HeightmapLayer{ heightmap, mode });

	if (mode == pcb::LayerMode::Addition) {
		add(heightmap);
	}
	else if (mode == pcb::LayerMode::Subtraction) {
		subtract(heightmap);
	}
}

int pcb::LayeredHeightmap::getLayerCount() const {
	return layers.size();
}

std::vector<pcb::HeightmapLayer> pcb::LayeredHeightmap::getLayers() const {
	return layers;
}