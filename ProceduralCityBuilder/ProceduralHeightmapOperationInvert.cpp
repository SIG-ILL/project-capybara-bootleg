#include "ProceduralHeightmapOperationInvert.hpp"

pcb::ProceduralHeightmapOperationInvert::ProceduralHeightmapOperationInvert(std::shared_ptr<ProceduralHeightmapOperation> sourceToModify) : sourceToModify(sourceToModify) {}

std::unique_ptr<pcb::Heightmap> pcb::ProceduralHeightmapOperationInvert::generateResult() const {
	std::unique_ptr<Heightmap> heightmap = sourceToModify->generateResult();
	heightmap->invert();

	return heightmap;
}

int pcb::ProceduralHeightmapOperationInvert::getHeightmapWidth() const {
	return sourceToModify->getHeightmapWidth();
}

int pcb::ProceduralHeightmapOperationInvert::getHeightmapHeight() const {
	return sourceToModify->getHeightmapHeight();
}