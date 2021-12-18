#include "ProceduralHeightmapOperationAddMountains.hpp"

#include "ProceduralHeightmapOperationAbsoluteNoise.hpp"
#include "ProceduralHeightmapOperationInvert.hpp"
#include "ProceduralHeightmapOperationNoise.hpp"
#include "ProceduralHeightmapOperationRoughen.hpp"
#include "ProceduralHeightmapOperationRectangle.hpp"
#include "ProceduralHeightmapOperationMask.hpp"
#include "ProceduralHeightmapOperationMaximum.hpp"

pcb::ProceduralHeightmapOperationAddMountains::ProceduralHeightmapOperationAddMountains(std::shared_ptr<ProceduralHeightmapOperation> sourceToModify, int offsetFromCenterX, int offsetFromCenterY)
	: sourceToModify(sourceToModify), offsetFromCenterX(offsetFromCenterX), offsetFromCenterY(offsetFromCenterY) {}

std::unique_ptr<pcb::Heightmap> pcb::ProceduralHeightmapOperationAddMountains::generateResult() const {
	int width = sourceToModify->getHeightmapWidth();
	int height = sourceToModify->getHeightmapHeight();

	std::unique_ptr<ProceduralHeightmapOperationAbsoluteNoise> baseNoise = std::make_unique<ProceduralHeightmapOperationAbsoluteNoise>(width, height, 196, 196, 0, 0);
	std::unique_ptr<ProceduralHeightmapOperationInvert> invertedNoise = std::make_unique<ProceduralHeightmapOperationInvert>(std::move(baseNoise));
	std::unique_ptr<ProceduralHeightmapOperationNoise> roughenerNoise = std::make_unique<ProceduralHeightmapOperationNoise>(width, height, 32, 32, 32, 32);
	std::unique_ptr<ProceduralHeightmapOperationRoughen> roughNoise = std::make_unique<ProceduralHeightmapOperationRoughen>(std::move(invertedNoise), std::move(roughenerNoise), 0.125);
	// Use an oval mask instead? Which first needs to be implemented...
	std::unique_ptr<ProceduralHeightmapOperationRectangle> mask = std::make_unique<ProceduralHeightmapOperationRectangle>(width, height, static_cast<int>(0.1 * width), static_cast<int>(0.0625 * height), static_cast<int>(0.1 * width), static_cast<int>(0.0625 * height), offsetFromCenterX, offsetFromCenterY);
	std::unique_ptr<ProceduralHeightmapOperationMask> maskedNoise = std::make_unique<ProceduralHeightmapOperationMask>(std::move(roughNoise), std::move(mask));

	std::unique_ptr<ProceduralHeightmapOperationMaximum> finalOperation = std::make_unique<ProceduralHeightmapOperationMaximum>(sourceToModify, std::move(maskedNoise));

	return finalOperation->generateResult();
}

int pcb::ProceduralHeightmapOperationAddMountains::getHeightmapWidth() const {
	return sourceToModify->getHeightmapWidth();
}

int pcb::ProceduralHeightmapOperationAddMountains::getHeightmapHeight() const {
	return sourceToModify->getHeightmapHeight();
}