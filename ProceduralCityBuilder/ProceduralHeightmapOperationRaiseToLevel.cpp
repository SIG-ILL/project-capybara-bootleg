#include "ProceduralHeightmapOperationRaiseToLevel.hpp"

pcb::ProceduralHeightmapOperationRaiseToLevel::ProceduralHeightmapOperationRaiseToLevel(std::shared_ptr<ProceduralHeightmapOperation> sourceToModify, const unsigned char level)
: sourceToModify(sourceToModify), targetLevel(level) {}

std::unique_ptr<pcb::Heightmap> pcb::ProceduralHeightmapOperationRaiseToLevel::generateResult() const {
	std::unique_ptr<Heightmap> heightmap = sourceToModify->generateResult();
	heightmap->raiseToLevel(targetLevel);

	return heightmap;
}