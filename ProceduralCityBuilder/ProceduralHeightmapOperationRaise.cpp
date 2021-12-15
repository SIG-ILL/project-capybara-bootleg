#include "ProceduralHeightmapOperationRaise.hpp"

pcb::ProceduralHeightmapOperationRaise::ProceduralHeightmapOperationRaise(std::shared_ptr<ProceduralHeightmapOperation> sourceToModify, unsigned char amount)
	: sourceToModify(sourceToModify), amount(amount) {}

std::unique_ptr<pcb::Heightmap> pcb::ProceduralHeightmapOperationRaise::generateResult() const {
	std::unique_ptr<Heightmap> heightmap = sourceToModify->generateResult();
	heightmap->raise(amount);

	return heightmap;
}