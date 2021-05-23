#pragma once

#include <vector>

#include "Heightmap.hpp"
#include "HeightmapLayer.hpp"

namespace pcb {
	class LayeredHeightmap final : public pcb::Heightmap {
	public:
		LayeredHeightmap(int width, int height, unsigned char* elevationValues);
		LayeredHeightmap(const pcb::Heightmap& baseHeightmap);

		pcb::LayeredHeightmap& operator+=(const pcb::LayeredHeightmap& other);
		pcb::LayeredHeightmap& operator+=(const pcb::Heightmap& other);
		pcb::LayeredHeightmap& operator-=(const pcb::LayeredHeightmap& other);		
		pcb::LayeredHeightmap& operator-=(const pcb::Heightmap& other);

		int getWidth() const override;
		int getHeight() const override;

		void addLayer(const pcb::Heightmap& heightmap, pcb::LayerMode mode);
		int getLayerCount() const;
		std::vector<pcb::HeightmapLayer> getLayers() const;

	private:
		std::vector<HeightmapLayer> layers;
	};
}