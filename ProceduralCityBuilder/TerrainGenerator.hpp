#pragma once

#include "Terrain.hpp"
#include "HeightMap.hpp"

namespace pcb {
	class TerrainGenerator {
	public:
		TerrainGenerator();
		~TerrainGenerator();

		pcb::Terrain* generateNew();
		pcb::Image* getHeightMap24BitImageNew();

	private:
		pcb::HeightMap* heightMap;
	};
}