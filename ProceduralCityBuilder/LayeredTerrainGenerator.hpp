#pragma once

#include "LayeredTerrain.hpp"

namespace pcb {
	class LayeredTerrainGenerator final {
	public:
		LayeredTerrainGenerator(int mapWidth, int mapHeight, double scale);
		~LayeredTerrainGenerator();

		pcb::LayeredTerrain* generateNew();
		pcb::Image* getHeightmap24BitImageNew() const;
		std::vector<pcb::Image*> getHeightmapLayers24BitImagesNew() const;

	private:
		int mapWidth;
		int mapHeight;
		double scale;
		pcb::LayeredHeightmap* heightmap;
	};
}