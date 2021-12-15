#pragma once

#include "ProceduralHeightmapOperation.hpp"

namespace pcb {
	class ProceduralHeightmapOperationLowerToLevel : public ProceduralHeightmapOperation {
	public:
		ProceduralHeightmapOperationLowerToLevel(std::shared_ptr<ProceduralHeightmapOperation> sourceToModify, const unsigned char level);

		std::unique_ptr<Heightmap> generateResult() const override;

	private:
		std::shared_ptr<ProceduralHeightmapOperation> sourceToModify;
		unsigned char targetLevel;
	};
}