#include "ProceduralHeightmapOperationModifierCacheResult.hpp"

pcb::ProceduralHeightmapOperationModifierCacheResult::ProceduralHeightmapOperationModifierCacheResult(const ProceduralHeightmapOperation& operation) :
	cachedResult(operation.generateResult()) {}

std::unique_ptr<pcb::Heightmap> pcb::ProceduralHeightmapOperationModifierCacheResult::generateResult() const {
	return std::make_unique<Heightmap>(*cachedResult);
}