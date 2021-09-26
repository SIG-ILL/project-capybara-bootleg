#pragma once

#include <memory>

#include "LayeredTerrain.hpp"
#include "HeightmapImage.hpp"

namespace pcb {
	class LayeredTerrainGenerator {
	public:
		LayeredTerrainGenerator(int mapWidth, int mapHeight, double scale);

		virtual std::unique_ptr<LayeredTerrain> generate();
		std::unique_ptr<LayeredTerrain> generateRandom();
		virtual std::unique_ptr<HeightmapImage> getHeightmap24BitImage() const;

	protected:
		int mapWidth;
		int mapHeight;
		double scale;
		std::unique_ptr<LayeredHeightmap> heightmap;
	};
}