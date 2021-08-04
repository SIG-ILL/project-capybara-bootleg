#pragma once

#include <vector>

#include "LayeredHeightmap.hpp"
#include "NoiseGenerator.hpp"

namespace pcb {
	class LayeredHeightmapGenerator final {
	public:
		LayeredHeightmapGenerator(int mapWidth, int mapHeight);

		pcb::LayeredHeightmap* generateNew() const;
		pcb::LayeredHeightmap* generateNewRandom() const;

	private:
		int mapWidth;
		int mapHeight;
		pcb::NoiseGenerator noiseGenerator;

		pcb::Heightmap generateHeightmap(double noiseSamplingFrequencyX, double noiseSamplingFrequencyY, double xOffset, double yOffset) const;
		double generateElevationForNoiseCoordinates(double x, double y) const;
	};
}