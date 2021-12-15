#pragma once

#include "ProceduralHeightmapOperation.hpp"

namespace pcb {
	class ProceduralHeightmapOperationClamp : public ProceduralHeightmapOperation {
	public:
		ProceduralHeightmapOperationClamp(std::shared_ptr<ProceduralHeightmapOperation> sourceToModify, const unsigned char lowerLevel, const unsigned char upperLevel);

		std::unique_ptr<Heightmap> generateResult() const override;

	private:
		std::shared_ptr<ProceduralHeightmapOperation> sourceToModify;
		unsigned char targetLowerLevel;
		unsigned char targetUpperLevel;
	};
}