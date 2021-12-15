#include "ProceduralHeightmapOperationScale.hpp"

pcb::ProceduralHeightmapOperationScale::ProceduralHeightmapOperationScale(std::shared_ptr<ProceduralHeightmapOperation> sourceToModify, const double factor)
	: sourceToModify(sourceToModify), factor(factor) {}

std::unique_ptr<pcb::Heightmap> pcb::ProceduralHeightmapOperationScale::generateResult() const {
	std::unique_ptr<Heightmap> heightmap = sourceToModify->generateResult();
	heightmap->scale(factor);

	return heightmap;
}