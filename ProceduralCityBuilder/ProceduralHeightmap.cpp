#include "ProceduralHeightmap.hpp"

pcb::ProceduralHeightmap::ProceduralHeightmap(std::unique_ptr<ProceduralHeightmapOperation> topLevelOperation) : topLevelOperation(std::move(topLevelOperation)) {}

pcb::ProceduralHeightmap::ProceduralHeightmap(const std::string& stringDefinition) {
	// TODO: Parse string and create operations from it.
}

std::unique_ptr<pcb::Heightmap> pcb::ProceduralHeightmap::generateResult() {
	return topLevelOperation->generateResult();
}