#include "ProceduralHeightmapOperationToBlackAndWhite.hpp"

pcb::ProceduralHeightmapOperationToBlackAndWhite::ProceduralHeightmapOperationToBlackAndWhite(std::shared_ptr<ProceduralHeightmapOperation> sourceToModify, unsigned char inclusiveThreshold)
	: sourceToModify(sourceToModify), inclusiveThreshold(inclusiveThreshold) {}

std::unique_ptr<pcb::Heightmap> pcb::ProceduralHeightmapOperationToBlackAndWhite::generateResult() const {
	std::unique_ptr<Heightmap> heightmap = sourceToModify->generateResult();
	heightmap->toBlackAndWhite(inclusiveThreshold);

	return heightmap;
}