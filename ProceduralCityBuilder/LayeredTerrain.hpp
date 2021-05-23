#pragma once

#include <vector>

#include "Terrain.hpp"
#include "LayeredHeightmap.hpp"

namespace pcb {
	class LayeredTerrain final : public pcb::Terrain {
	public:
		LayeredTerrain(const pcb::LayeredHeightmap& layeredHeightmap, double scale);

		std::vector<pcb::Terrain> getLayers() const;
		int getLayerCount() const;

	private:
		std::vector<pcb::Terrain> layers;
	};
}