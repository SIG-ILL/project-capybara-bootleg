#pragma once

#include "ProceduralHeightmapOperation.hpp"

namespace pcb {
	/*
	* Cached the result of the provided operation and returns a copy of the cached heightmap.
	* This prevents generating the exact same data again when reusing an operation.
	* In order to comply with the const-ness of the generateResult function, generation of the result is done on construction instead of during a call to generateResult,
	* making this different in timing of computational work than the other operations.
	*/
	class ProceduralHeightmapOperationModifierCacheResult final : public ProceduralHeightmapOperation {
	public:
		ProceduralHeightmapOperationModifierCacheResult(const ProceduralHeightmapOperation& operation);

		std::unique_ptr<Heightmap> generateResult() const override;
		int getHeightmapWidth() const override;
		int getHeightmapHeight() const override;

	private:
		std::unique_ptr<Heightmap> cachedResult;
	};
}