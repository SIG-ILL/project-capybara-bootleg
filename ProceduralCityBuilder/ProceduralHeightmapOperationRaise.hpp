#pragma once

#include "ProceduralHeightmapOperation.hpp"

namespace pcb {
	class ProceduralHeightmapOperationRaise final : public ProceduralHeightmapOperation {
	public:
		ProceduralHeightmapOperationRaise(std::shared_ptr<ProceduralHeightmapOperation> sourceToModify, unsigned char amount);

		std::unique_ptr<Heightmap> generateResult() const override;
		int getHeightmapWidth() const override;
		int getHeightmapHeight() const override;

	private:
		std::shared_ptr<ProceduralHeightmapOperation> sourceToModify;
		unsigned char amount;
	};
}