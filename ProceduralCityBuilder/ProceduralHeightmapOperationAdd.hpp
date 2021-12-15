#pragma once

#include "ProceduralHeightmapOperation.hpp"

namespace pcb {
	class ProceduralHeightmapOperationAdd final : public ProceduralHeightmapOperation {
	public:
		ProceduralHeightmapOperationAdd(std::shared_ptr<ProceduralHeightmapOperation> firstOperand, std::shared_ptr<ProceduralHeightmapOperation> secondOperand);

		std::unique_ptr<Heightmap> generateResult() const override;

	private:
		std::shared_ptr<ProceduralHeightmapOperation> firstOperand;
		std::shared_ptr<ProceduralHeightmapOperation> secondOperand;
	};
}