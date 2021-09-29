#include "LayeredHeightmap.hpp"

pcb::LayeredHeightmap::LayeredHeightmap(int width, int height) : Heightmap(width, height), layers(std::make_shared<std::vector<std::shared_ptr<HeightmapLayer>>>()) {}

pcb::LayeredHeightmap::LayeredHeightmap(int width, int height, std::shared_ptr<std::vector<unsigned char>> elevationValues) : Heightmap(width, height, elevationValues), layers(std::make_shared<std::vector<std::shared_ptr<HeightmapLayer>>>(1, std::make_shared<HeightmapLayer>(std::make_shared<Heightmap>(width, height, elevationValues), pcb::LayerMode::Addition))) {}

pcb::LayeredHeightmap::LayeredHeightmap(const pcb::Heightmap& baseHeightmap) : Heightmap(baseHeightmap), layers(std::make_shared<std::vector<std::shared_ptr<HeightmapLayer>>>(1, std::make_shared<HeightmapLayer>(std::make_shared<Heightmap>(baseHeightmap), pcb::LayerMode::Addition))) {}

pcb::LayeredHeightmap& pcb::LayeredHeightmap::operator+=(const pcb::Heightmap& other) {
	addLayer(std::make_shared<Heightmap>(other), pcb::LayerMode::Addition);
	return *this;
}

pcb::LayeredHeightmap& pcb::LayeredHeightmap::operator+=(const pcb::LayeredHeightmap& other) {
	addLayer(std::make_shared<LayeredHeightmap>(other), pcb::LayerMode::Addition);
	return *this;
}

pcb::LayeredHeightmap& pcb::LayeredHeightmap::operator-=(const pcb::Heightmap& other) {
	addLayer(std::make_shared<Heightmap>(other), pcb::LayerMode::Subtraction);
	return *this;
}

pcb::LayeredHeightmap& pcb::LayeredHeightmap::operator-=(const pcb::LayeredHeightmap& other) {
	addLayer(std::make_shared<LayeredHeightmap>(other), pcb::LayerMode::Subtraction);
	return *this;
}

void pcb::LayeredHeightmap::addLayer(std::shared_ptr<HeightmapLayer> heightmapLayer) {
	layers->push_back(heightmapLayer);

	if (heightmapLayer->mode == LayerMode::Addition) {
		add(*(heightmapLayer->heightmap));
	}
	else if (heightmapLayer->mode == LayerMode::Subtraction) {
		subtract(*(heightmapLayer->heightmap));
	}
	else if (heightmapLayer->mode == LayerMode::Mask) {
		mask(*(heightmapLayer->heightmap));
	}
}

void pcb::LayeredHeightmap::addLayer(std::shared_ptr<Heightmap> heightmap, LayerMode mode) {
	addLayer(std::make_shared<HeightmapLayer>(heightmap, mode));
}

int pcb::LayeredHeightmap::getLayerCount() const {
	return layers->size();
}

std::shared_ptr<std::vector<std::shared_ptr<pcb::HeightmapLayer>>> pcb::LayeredHeightmap::getLayers() const {
	return layers;
}