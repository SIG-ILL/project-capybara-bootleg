#include "ProceduralHeightmapOperationClamp.hpp"

pcb::ProceduralHeightmapOperationClamp::ProceduralHeightmapOperationClamp(std::shared_ptr<ProceduralHeightmapOperation> sourceToModify, const unsigned char lowerLevel, const unsigned char upperLevel)
	: sourceToModify(sourceToModify), targetLowerLevel(lowerLevel), targetUpperLevel(upperLevel) {}

std::unique_ptr<pcb::Heightmap> pcb::ProceduralHeightmapOperationClamp::generateResult() const {
	std::unique_ptr<Heightmap> heightmap = sourceToModify->generateResult();
	heightmap->clamp(targetLowerLevel, targetUpperLevel);

	return heightmap;
}

int pcb::ProceduralHeightmapOperationClamp::getHeightmapWidth() const {
	return sourceToModify->getHeightmapWidth();
}

int pcb::ProceduralHeightmapOperationClamp::getHeightmapHeight() const {
	return sourceToModify->getHeightmapHeight();
}