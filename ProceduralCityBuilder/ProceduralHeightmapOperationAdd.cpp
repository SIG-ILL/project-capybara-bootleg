#include "ProceduralHeightmapOperationAdd.hpp"

pcb::ProceduralHeightmapOperationAdd::ProceduralHeightmapOperationAdd(std::shared_ptr<ProceduralHeightmapOperation> firstOperand, std::shared_ptr<ProceduralHeightmapOperation> secondOperand)
	: firstOperand(firstOperand), secondOperand(secondOperand) {}

std::unique_ptr<pcb::Heightmap> pcb::ProceduralHeightmapOperationAdd::generateResult() const {
	std::unique_ptr<Heightmap> firstResult = firstOperand->generateResult();
	std::unique_ptr<Heightmap> secondResult = secondOperand->generateResult();

	firstResult->add(*secondResult);

	return firstResult;
}

int pcb::ProceduralHeightmapOperationAdd::getHeightmapWidth() const {
	return firstOperand->getHeightmapWidth();
}

int pcb::ProceduralHeightmapOperationAdd::getHeightmapHeight() const {
	return firstOperand->getHeightmapHeight();
}