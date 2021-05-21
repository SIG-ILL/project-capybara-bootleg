#pragma once

#include "Terrain.hpp"
#include "Heightmap.hpp"

namespace pcb {
	class TerrainGenerator final {
	public:
		TerrainGenerator(int mapWidth, int mapHeight, double scale);
		~TerrainGenerator();

		pcb::Terrain* generateNew();
		pcb::Image* getHeightmap24BitImageNew() const;

	private:
		int mapWidth;
		int mapHeight;
		double scale;
		pcb::Heightmap* heightmap;
	};
}