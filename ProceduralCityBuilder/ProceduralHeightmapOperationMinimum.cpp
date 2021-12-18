#include "ProceduralHeightmapOperationMinimum.hpp"

pcb::ProceduralHeightmapOperationMinimum::ProceduralHeightmapOperationMinimum(std::shared_ptr<ProceduralHeightmapOperation> firstOperand, std::shared_ptr<ProceduralHeightmapOperation> secondOperand)
	: firstOperand(firstOperand), secondOperand(secondOperand) {}

std::unique_ptr<pcb::Heightmap> pcb::ProceduralHeightmapOperationMinimum::generateResult() const {
	std::unique_ptr<Heightmap> firstResult = firstOperand->generateResult();
	std::unique_ptr<Heightmap> secondResult = secondOperand->generateResult();

	firstResult->minimum(*secondResult);

	return firstResult;
}

int pcb::ProceduralHeightmapOperationMinimum::getHeightmapWidth() const {
	return firstOperand->getHeightmapWidth();
}

int pcb::ProceduralHeightmapOperationMinimum::getHeightmapHeight() const {
	return firstOperand->getHeightmapHeight();
}