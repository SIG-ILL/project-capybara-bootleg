#pragma once

#include "Heightmap.hpp"

namespace pcb {
	enum class LayerMode { Addition, Subtraction };

	class HeightmapLayer final {
	public:
		pcb::Heightmap heightmap;
		pcb::LayerMode mode;
	};
}