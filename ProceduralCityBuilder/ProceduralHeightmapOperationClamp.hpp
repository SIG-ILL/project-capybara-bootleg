#pragma once

#include "ProceduralHeightmapOperation.hpp"

namespace pcb {
	class ProceduralHeightmapOperationClamp final : public ProceduralHeightmapOperation {
	public:
		ProceduralHeightmapOperationClamp(std::shared_ptr<ProceduralHeightmapOperation> sourceToModify, const unsigned char lowerLevel, const unsigned char upperLevel);

		std::unique_ptr<Heightmap> generateResult() const override;
		int getHeightmapWidth() const override;
		int getHeightmapHeight() const override;

	private:
		std::shared_ptr<ProceduralHeightmapOperation> sourceToModify;
		unsigned char targetLowerLevel;
		unsigned char targetUpperLevel;
	};
}