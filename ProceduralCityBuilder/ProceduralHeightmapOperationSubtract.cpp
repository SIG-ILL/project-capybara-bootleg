#include "ProceduralHeightmapOperationSubtract.hpp"

pcb::ProceduralHeightmapOperationSubtract::ProceduralHeightmapOperationSubtract(std::shared_ptr<ProceduralHeightmapOperation> firstOperand, std::shared_ptr<ProceduralHeightmapOperation> secondOperand)
	: firstOperand(firstOperand), secondOperand(secondOperand) {}

std::unique_ptr<pcb::Heightmap> pcb::ProceduralHeightmapOperationSubtract::generateResult() const {
	std::unique_ptr<Heightmap> firstResult = firstOperand->generateResult();
	std::unique_ptr<Heightmap> secondResult = secondOperand->generateResult();

	firstResult->subtract(*secondResult);

	return firstResult;
}

int pcb::ProceduralHeightmapOperationSubtract::getHeightmapWidth() const {
	return firstOperand->getHeightmapWidth();
}

int pcb::ProceduralHeightmapOperationSubtract::getHeightmapHeight() const {
	return firstOperand->getHeightmapHeight();
}