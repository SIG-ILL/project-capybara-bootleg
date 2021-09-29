#pragma once

#include <vector>
#include <memory>

#include "LayeredHeightmap.hpp"
#include "NoiseGenerator.hpp"

namespace pcb {
	class LayeredHeightmapGenerator final {
	public:
		LayeredHeightmapGenerator(int mapWidth, int mapHeight);

		std::unique_ptr<LayeredHeightmap> generate() const;
		std::unique_ptr<LayeredHeightmap> generateRandom() const;

	private:
		int mapWidth;
		int mapHeight;
		NoiseGenerator noiseGenerator;

		std::unique_ptr<Heightmap> generateHeightmap(double noiseSamplingFrequencyX, double noiseSamplingFrequencyY, double xOffset, double yOffset) const;
		double generateElevationForNoiseCoordinates(double x, double y) const;
	};
}