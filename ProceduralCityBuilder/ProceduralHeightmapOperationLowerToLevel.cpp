#include "ProceduralHeightmapOperationLowerToLevel.hpp"

pcb::ProceduralHeightmapOperationLowerToLevel::ProceduralHeightmapOperationLowerToLevel(std::shared_ptr<ProceduralHeightmapOperation> sourceToModify, const unsigned char level)
	: sourceToModify(sourceToModify), targetLevel(level) {}

std::unique_ptr<pcb::Heightmap> pcb::ProceduralHeightmapOperationLowerToLevel::generateResult() const {
	std::unique_ptr<Heightmap> heightmap = sourceToModify->generateResult();
	heightmap->lowerToLevel(targetLevel);

	return heightmap;
}