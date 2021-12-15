#include "ProceduralHeightmapOperationScaleAmplitude.hpp"

pcb::ProceduralHeightmapOperationScaleAmplitude::ProceduralHeightmapOperationScaleAmplitude(std::shared_ptr<ProceduralHeightmapOperation> sourceToModify, const double factor)
	: sourceToModify(sourceToModify), factor(factor) {}

std::unique_ptr<pcb::Heightmap> pcb::ProceduralHeightmapOperationScaleAmplitude::generateResult() const {
	std::unique_ptr<Heightmap> heightmap = sourceToModify->generateResult();
	heightmap->scaleAmplitude(factor);

	return heightmap;
}