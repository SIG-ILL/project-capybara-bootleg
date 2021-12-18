#pragma once

#include "ProceduralHeightmapOperation.hpp"

namespace pcb {
	class ProceduralHeightmapOperationRoughen final : public ProceduralHeightmapOperation {
	public:
		ProceduralHeightmapOperationRoughen(std::shared_ptr<ProceduralHeightmapOperation> firstOperand, std::shared_ptr<ProceduralHeightmapOperation> secondOperand, double scale);

		std::unique_ptr<Heightmap> generateResult() const override;
		int getHeightmapWidth() const override;
		int getHeightmapHeight() const override;

	private:
		std::shared_ptr<ProceduralHeightmapOperation> firstOperand;
		std::shared_ptr<ProceduralHeightmapOperation> secondOperand;
		double scale;
	};
}