#include "ProceduralHeightmapOperationAddMountains.hpp"

#include "ProceduralHeightmapOperationAbsoluteNoise.hpp"
#include "ProceduralHeightmapOperationInvert.hpp"
#include "ProceduralHeightmapOperationNoise.hpp"
#include "ProceduralHeightmapOperationRoughen.hpp"
#include "ProceduralHeightmapOperationEllipse.hpp"
#include "ProceduralHeightmapOperationMask.hpp"
#include "ProceduralHeightmapOperationMaximum.hpp"
#include "ProceduralHeightmapOperationScale.hpp"

pcb::ProceduralHeightmapOperationAddMountains::ProceduralHeightmapOperationAddMountains(std::shared_ptr<ProceduralHeightmapOperation> sourceToModify, int offsetFromCenterX, int offsetFromCenterY)
	: sourceToModify(sourceToModify), offsetFromCenterX(offsetFromCenterX), offsetFromCenterY(offsetFromCenterY) {}

std::unique_ptr<pcb::Heightmap> pcb::ProceduralHeightmapOperationAddMountains::generateResult() const {
	int width = sourceToModify->getHeightmapWidth();
	int height = sourceToModify->getHeightmapHeight();

	auto baseNoise = std::make_unique<ProceduralHeightmapOperationAbsoluteNoise>(width, height, 392, 196, 0, 0);
	auto invertedNoise = std::make_unique<ProceduralHeightmapOperationInvert>(std::move(baseNoise));
	auto roughenerNoise = std::make_unique<ProceduralHeightmapOperationNoise>(width, height, 64, 32, 32, 32);
	auto roughNoise = std::make_unique<ProceduralHeightmapOperationRoughen>(std::move(invertedNoise), std::move(roughenerNoise), 0.125);
	auto roughenerNoise2 = std::make_unique<ProceduralHeightmapOperationNoise>(width, height, 32, 16, 16, 16);
	auto roughNoise2 = std::make_unique<ProceduralHeightmapOperationRoughen>(std::move(roughNoise), std::move(roughenerNoise2), 0.0625);
	auto roughenerNoise3 = std::make_unique<ProceduralHeightmapOperationNoise>(width, height, 16, 8, 8, 8);
	auto roughNoise3 = std::make_unique<ProceduralHeightmapOperationRoughen>(std::move(roughNoise2), std::move(roughenerNoise3), 0.03125);

	auto mask = std::make_unique<ProceduralHeightmapOperationEllipse>(width, height, static_cast<int>(0.2 * width), static_cast<int>(0.0625 * height), 2.0, offsetFromCenterX, offsetFromCenterY);
	auto maskedNoise = std::make_unique<ProceduralHeightmapOperationMask>(std::move(roughNoise3), std::move(mask));

	std::unique_ptr<Heightmap> mountainsResult = maskedNoise->generateResult();
	std::unique_ptr<Heightmap> sourceResult = sourceToModify->generateResult();
	const float TARGET_MOUNTAIN_MAX_HEIGHT_FACTOR = 1.35f;
	float scalingFactor = (TARGET_MOUNTAIN_MAX_HEIGHT_FACTOR / (static_cast<float>(mountainsResult->getHighestElevation()) / static_cast<float>(sourceResult->getHighestElevation())));	// TODO: Make ellipse mask falloff linked to this factor (higher factor = higher falloff, lower factor = lower falloff) to improve blending of mountains into source and reduce steepness at high scaling factors?
	mountainsResult->scale(scalingFactor);
	sourceResult->maximum(*mountainsResult);

	return sourceResult;
}

int pcb::ProceduralHeightmapOperationAddMountains::getHeightmapWidth() const {
	return sourceToModify->getHeightmapWidth();
}

int pcb::ProceduralHeightmapOperationAddMountains::getHeightmapHeight() const {
	return sourceToModify->getHeightmapHeight();
}