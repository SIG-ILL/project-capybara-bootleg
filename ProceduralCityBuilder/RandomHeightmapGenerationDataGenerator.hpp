#pragma once

#include "RandomHeightmapGenerationData.hpp"

namespace pcb {
	class RandomHeightmapGenerationDataGenerator final {
	public:
		RandomHeightmapGenerationData generate(int mapWidth, int mapHeight) const;
	};
}