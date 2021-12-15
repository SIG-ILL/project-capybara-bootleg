#pragma once

#include "ProceduralHeightmapOperation.hpp"

namespace pcb {
	class ProceduralHeightmapOperationModifierCacheResult final : public ProceduralHeightmapOperation {
	public:
		ProceduralHeightmapOperationModifierCacheResult(const ProceduralHeightmapOperation& operation);

		std::unique_ptr<Heightmap> generateResult() const override;

	private:
		std::unique_ptr<Heightmap> cachedResult;
	};
}