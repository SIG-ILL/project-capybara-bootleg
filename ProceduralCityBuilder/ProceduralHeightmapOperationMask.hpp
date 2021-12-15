#pragma once

#include "ProceduralHeightmapOperation.hpp"

namespace pcb {
	class ProceduralHeightmapOperationMask final : public ProceduralHeightmapOperation {
	public:
		ProceduralHeightmapOperationMask(std::shared_ptr<ProceduralHeightmapOperation> firstOperand, std::shared_ptr<ProceduralHeightmapOperation> secondOperand);

		std::unique_ptr<Heightmap> generateResult() const override;

	private:
		std::shared_ptr<ProceduralHeightmapOperation> firstOperand;
		std::shared_ptr<ProceduralHeightmapOperation> secondOperand;
	};
}