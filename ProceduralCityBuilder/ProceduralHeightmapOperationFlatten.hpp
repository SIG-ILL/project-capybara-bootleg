#pragma once

#include "ProceduralHeightmapOperation.hpp"

namespace pcb {
	class ProceduralHeightmapOperationFlatten final : public ProceduralHeightmapOperation {
	public:
		ProceduralHeightmapOperationFlatten(std::shared_ptr<ProceduralHeightmapOperation> sourceToModify, const unsigned char amountOfBuckets);

		std::unique_ptr<Heightmap> generateResult() const override;
		int getHeightmapWidth() const override;
		int getHeightmapHeight() const override;

	private:
		std::shared_ptr<ProceduralHeightmapOperation> sourceToModify;
		unsigned char amountOfBuckets;
	};
}