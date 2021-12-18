#pragma once

#include "ProceduralHeightmapOperation.hpp"

namespace pcb {
	class ProceduralHeightmapOperationRaiseToLevel final : public ProceduralHeightmapOperation {
	public:
		ProceduralHeightmapOperationRaiseToLevel(std::shared_ptr<ProceduralHeightmapOperation> sourceToModify, const unsigned char level);

		std::unique_ptr<Heightmap> generateResult() const override;
		int getHeightmapWidth() const override;
		int getHeightmapHeight() const override;

	private:
		std::shared_ptr<ProceduralHeightmapOperation> sourceToModify;
		unsigned char targetLevel;
	};
}