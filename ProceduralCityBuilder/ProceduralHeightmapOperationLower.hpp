#pragma once

#include "ProceduralHeightmapOperation.hpp"

namespace pcb {
	class ProceduralHeightmapOperationLower final : public ProceduralHeightmapOperation {
	public:
		ProceduralHeightmapOperationLower(std::shared_ptr<ProceduralHeightmapOperation> sourceToModify, unsigned char amount);

		std::unique_ptr<Heightmap> generateResult() const override;
		int getHeightmapWidth() const override;
		int getHeightmapHeight() const override;

	private:
		std::shared_ptr<ProceduralHeightmapOperation> sourceToModify;
		unsigned char amount;
	};
}