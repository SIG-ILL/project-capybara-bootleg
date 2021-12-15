#pragma once

#include "HeightMap.hpp"

namespace pcb {
	class ProceduralHeightmapOperation {
	public:
		virtual std::unique_ptr<Heightmap> generateResult() const = 0;
	};
}