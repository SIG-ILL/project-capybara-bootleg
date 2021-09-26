#pragma once

#include <vector>
#include <memory>

#include "LayeredHeightmap.hpp"
#include "NoiseGenerator.hpp"

namespace pcb {
	class LayeredHeightmapGenerator final {
	public:
		LayeredHeightmapGenerator(int mapWidth, int mapHeight);

		LayeredHeightmap generate() const;
		LayeredHeightmap generateRandom() const;

	private:
		int mapWidth;
		int mapHeight;
		NoiseGenerator noiseGenerator;

		Heightmap generateHeightmap(double noiseSamplingFrequencyX, double noiseSamplingFrequencyY, double xOffset, double yOffset) const;
		double generateElevationForNoiseCoordinates(double x, double y) const;
	};
}