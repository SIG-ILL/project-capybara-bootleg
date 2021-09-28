#pragma once

#include <memory>

#include "Heightmap.hpp"

namespace pcb {
	enum class LayerMode { Addition, Subtraction, Mask };

	class HeightmapLayer final {
	public:
		HeightmapLayer(std::shared_ptr<Heightmap> heightmap, LayerMode layerMode) : heightmap(heightmap), mode(layerMode) {}

		std::shared_ptr<Heightmap> heightmap;
		LayerMode mode;
	};
}