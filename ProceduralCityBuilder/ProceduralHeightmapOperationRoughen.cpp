#include "ProceduralHeightmapOperationRoughen.hpp"

pcb::ProceduralHeightmapOperationRoughen::ProceduralHeightmapOperationRoughen(std::shared_ptr<ProceduralHeightmapOperation> firstOperand, std::shared_ptr<ProceduralHeightmapOperation> secondOperand, double scale)
	: firstOperand(firstOperand), secondOperand(secondOperand), scale(scale) {}

std::unique_ptr<pcb::Heightmap> pcb::ProceduralHeightmapOperationRoughen::generateResult() const {
	std::unique_ptr<Heightmap> firstResult = firstOperand->generateResult();
	std::unique_ptr<Heightmap> secondResult = secondOperand->generateResult();

	firstResult->roughen(*secondResult, scale);

	return firstResult;
}

int pcb::ProceduralHeightmapOperationRoughen::getHeightmapWidth() const {
	return firstOperand->getHeightmapWidth();
}

int pcb::ProceduralHeightmapOperationRoughen::getHeightmapHeight() const {
	return firstOperand->getHeightmapHeight();
}