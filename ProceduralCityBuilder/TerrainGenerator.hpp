#pragma once

#include "LayeredTerrainGenerator.hpp"
#include "Terrain.hpp"
#include "Heightmap.hpp"
#include "HeightmapImage.hpp"

namespace pcb {
	class TerrainGenerator final : public LayeredTerrainGenerator {
	public:
		TerrainGenerator(int mapWidth, int mapHeight, double scale);

		std::unique_ptr<LayeredTerrain> generate() override;
		std::unique_ptr<HeightmapImage> getHeightmap24BitImage() const override;	
	};
}