#include "ProceduralHeightmapOperationMaximum.hpp"

pcb::ProceduralHeightmapOperationMaximum::ProceduralHeightmapOperationMaximum(std::shared_ptr<ProceduralHeightmapOperation> firstOperand, std::shared_ptr<ProceduralHeightmapOperation> secondOperand)
	: firstOperand(firstOperand), secondOperand(secondOperand) {}

std::unique_ptr<pcb::Heightmap> pcb::ProceduralHeightmapOperationMaximum::generateResult() const {
	std::unique_ptr<Heightmap> firstResult = firstOperand->generateResult();
	std::unique_ptr<Heightmap> secondResult = secondOperand->generateResult();

	firstResult->maximum(*secondResult);

	return firstResult;
}

int pcb::ProceduralHeightmapOperationMaximum::getHeightmapWidth() const {
	return firstOperand->getHeightmapWidth();
}

int pcb::ProceduralHeightmapOperationMaximum::getHeightmapHeight() const {
	return firstOperand->getHeightmapHeight();
}