#include "ProceduralHeightmapOperationFlatten.hpp"

pcb::ProceduralHeightmapOperationFlatten::ProceduralHeightmapOperationFlatten(std::shared_ptr<ProceduralHeightmapOperation> sourceToModify, const unsigned char amountOfBuckets)
	: sourceToModify(sourceToModify), amountOfBuckets(amountOfBuckets) {}

std::unique_ptr<pcb::Heightmap> pcb::ProceduralHeightmapOperationFlatten::generateResult() const {
	std::unique_ptr<Heightmap> heightmap = sourceToModify->generateResult();
	heightmap->flatten(amountOfBuckets);

	return heightmap;
}