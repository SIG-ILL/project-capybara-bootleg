#pragma once

#include "Terrain.hpp"
#include "Heightmap.hpp"

namespace pcb {
	class TerrainGenerator {
	public:
		TerrainGenerator();
		~TerrainGenerator();

		pcb::Terrain* generateNew();
		pcb::Image* getHeightmap24BitImageNew();

	private:
		pcb::Heightmap* heightmap;
	};
}