#pragma once

#include "Heightmap.hpp"

namespace pcb {
	enum class LayerMode { Addition, Subtraction, Mask };

	class HeightmapLayer final {
	public:
		HeightmapLayer(Heightmap heightmap, LayerMode layerMode) : heightmap(heightmap), mode(layerMode) {}

		pcb::Heightmap heightmap;
		pcb::LayerMode mode;
	};
}