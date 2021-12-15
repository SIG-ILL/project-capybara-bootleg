#include "ProceduralHeightmapOperationLower.hpp"

pcb::ProceduralHeightmapOperationLower::ProceduralHeightmapOperationLower(std::shared_ptr<ProceduralHeightmapOperation> sourceToModify, unsigned char amount)
	: sourceToModify(sourceToModify), amount(amount) {}

std::unique_ptr<pcb::Heightmap> pcb::ProceduralHeightmapOperationLower::generateResult() const {
	std::unique_ptr<Heightmap> heightmap = sourceToModify->generateResult();
	heightmap->lower(amount);

	return heightmap;
}