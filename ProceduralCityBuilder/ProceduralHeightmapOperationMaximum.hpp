#pragma once

#include "ProceduralHeightmapOperation.hpp"

namespace pcb {
	class ProceduralHeightmapOperationMaximum final : public ProceduralHeightmapOperation {
	public:
		ProceduralHeightmapOperationMaximum(std::shared_ptr<ProceduralHeightmapOperation> firstOperand, std::shared_ptr<ProceduralHeightmapOperation> secondOperand);

		std::unique_ptr<Heightmap> generateResult() const override;
		int getHeightmapWidth() const override;
		int getHeightmapHeight() const override;

	private:
		std::shared_ptr<ProceduralHeightmapOperation> firstOperand;
		std::shared_ptr<ProceduralHeightmapOperation> secondOperand;
	};
}