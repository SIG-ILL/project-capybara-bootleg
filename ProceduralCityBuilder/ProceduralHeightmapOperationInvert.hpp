#pragma once

#include "ProceduralHeightmapOperation.hpp"

namespace pcb {
	class ProceduralHeightmapOperationInvert final : public ProceduralHeightmapOperation {
	public:
		ProceduralHeightmapOperationInvert(std::shared_ptr<ProceduralHeightmapOperation> sourceToModify);

		std::unique_ptr<Heightmap> generateResult() const override;
		int getHeightmapWidth() const override;
		int getHeightmapHeight() const override;

	private:
		std::shared_ptr<ProceduralHeightmapOperation> sourceToModify;
	};
}