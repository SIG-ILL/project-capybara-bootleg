#pragma once

#include <vector>
#include <memory>

#include "Terrain.hpp"
#include "LayeredHeightmap.hpp"

namespace pcb {
	class LayeredTerrain final : public Terrain {
	public:
		LayeredTerrain(const LayeredHeightmap& layeredHeightmap, double scale);

		std::vector<std::shared_ptr<Terrain>> getLayers() const;
		int getLayerCount() const;

	private:
		std::vector<std::shared_ptr<Terrain>> layers;
	};
}