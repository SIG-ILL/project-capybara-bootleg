#pragma once

#include "ProceduralHeightmapOperation.hpp"

namespace pcb {
	class ProceduralHeightmapOperationRaise : public ProceduralHeightmapOperation {
	public:
		ProceduralHeightmapOperationRaise(std::shared_ptr<ProceduralHeightmapOperation> sourceToModify, unsigned char amount);

		std::unique_ptr<Heightmap> generateResult() const override;

	private:
		std::shared_ptr<ProceduralHeightmapOperation> sourceToModify;
		unsigned char amount;
	};
}