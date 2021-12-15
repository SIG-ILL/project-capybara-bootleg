#pragma once

#include "ProceduralHeightmapOperation.hpp"

namespace pcb {
	class ProceduralHeightmapOperationInvert : public ProceduralHeightmapOperation {
	public:
		ProceduralHeightmapOperationInvert(std::shared_ptr<ProceduralHeightmapOperation> sourceToModify);

		std::unique_ptr<Heightmap> generateResult() const override;

	private:
		std::shared_ptr<ProceduralHeightmapOperation> sourceToModify;
	};
}