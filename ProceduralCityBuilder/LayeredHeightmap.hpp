#pragma once

#include <vector>
#include <memory>

#include "Heightmap.hpp"
#include "HeightmapLayer.hpp"

namespace pcb {
	class LayeredHeightmap final : public pcb::Heightmap {
	public:
		LayeredHeightmap(int width, int height);
		LayeredHeightmap(int width, int height, std::shared_ptr<std::vector<unsigned char>> elevationValues);
		LayeredHeightmap(const Heightmap& baseHeightmap);

		LayeredHeightmap& operator+=(const LayeredHeightmap& other);
		LayeredHeightmap& operator+=(const Heightmap& other);
		LayeredHeightmap& operator-=(const LayeredHeightmap& other);		
		LayeredHeightmap& operator-=(const Heightmap& other);

		void addLayer(std::shared_ptr<HeightmapLayer> heightmapLayer);
		void addLayer(std::shared_ptr<Heightmap> heightmap, LayerMode mode);
		int getLayerCount() const;
		std::shared_ptr<std::vector<std::shared_ptr<HeightmapLayer>>> getLayers() const;

	private:
		std::shared_ptr<std::vector<std::shared_ptr<HeightmapLayer>>> layers;
	};
}