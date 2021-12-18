#pragma once

#include "ProceduralHeightmapOperation.hpp"

namespace pcb {
	class ProceduralHeightmapOperationAddMountains final : public ProceduralHeightmapOperation {
	public:
		ProceduralHeightmapOperationAddMountains(std::shared_ptr<ProceduralHeightmapOperation> sourceToModify, int offsetFromCenterX, int offsetFromCenterY);

		std::unique_ptr<Heightmap> generateResult() const override;
		int getHeightmapWidth() const override;
		int getHeightmapHeight() const override;

	private:
		std::shared_ptr<ProceduralHeightmapOperation> sourceToModify;
		int offsetFromCenterX;
		int offsetFromCenterY;
	};
}