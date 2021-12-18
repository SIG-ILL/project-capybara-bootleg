#pragma once

#include "ProceduralHeightmapOperation.hpp"

namespace pcb {
	class ProceduralHeightmapOperationToBlackAndWhite final : public ProceduralHeightmapOperation {
	public:
		ProceduralHeightmapOperationToBlackAndWhite(std::shared_ptr<ProceduralHeightmapOperation> sourceToModify, unsigned char inclusiveThreshold);

		std::unique_ptr<Heightmap> generateResult() const override;
		int getHeightmapWidth() const override;
		int getHeightmapHeight() const override;

	private:
		std::shared_ptr<ProceduralHeightmapOperation> sourceToModify;
		unsigned char inclusiveThreshold;
	};
}