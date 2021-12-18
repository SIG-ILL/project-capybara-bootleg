#include "ProceduralHeightmapOperationEmptyMap.hpp"

pcb::ProceduralHeightmapOperationEmptyMap::ProceduralHeightmapOperationEmptyMap(int width, int height)
	: width(width), height(height) {}

std::unique_ptr<pcb::Heightmap> pcb::ProceduralHeightmapOperationEmptyMap::generateResult() const {
	return std::make_unique<Heightmap>(width, height);
}

int pcb::ProceduralHeightmapOperationEmptyMap::getHeightmapWidth() const {
	return width;
}

int pcb::ProceduralHeightmapOperationEmptyMap::getHeightmapHeight() const {
	return height;
}