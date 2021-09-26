#pragma once

#include <vector>

#include "Heightmap.hpp"
#include "HeightmapLayer.hpp"

namespace pcb {
	class LayeredHeightmap final : public pcb::Heightmap {
	public:
		LayeredHeightmap(int width, int height);
		LayeredHeightmap(int width, int height, const std::vector<unsigned char> elevationValues);
		LayeredHeightmap(const Heightmap& baseHeightmap);

		LayeredHeightmap& operator+=(const LayeredHeightmap& other);
		LayeredHeightmap& operator+=(const Heightmap& other);
		LayeredHeightmap& operator-=(const LayeredHeightmap& other);		
		LayeredHeightmap& operator-=(const Heightmap& other);

		void addLayer(const HeightmapLayer& heightmapLayer);
		void addLayer(const Heightmap& heightmap, LayerMode mode);
		int getLayerCount() const;
		std::vector<HeightmapLayer> getLayers() const;

	private:
		std::vector<HeightmapLayer> layers;
	};
}