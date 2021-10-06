#pragma once

#include <vector>
#include <memory>

#include "LayeredHeightmap.hpp"
#include "NoiseGenerator.hpp"

namespace pcb {
	class LayeredHeightmapGenerator {
	public:
		LayeredHeightmapGenerator(int mapWidth, int mapHeight);

		virtual std::unique_ptr<LayeredHeightmap> generate() const;
		std::unique_ptr<LayeredHeightmap> generateRandom() const;

	protected:
		int mapWidth;
		int mapHeight;

		double generateElevationForNoiseCoordinates(double x, double y) const;

	private:		
		NoiseGenerator noiseGenerator;

		std::unique_ptr<Heightmap> generateHeightmap(double noiseSamplingFrequencyX, double noiseSamplingFrequencyY, double xOffset, double yOffset) const;		
	};
}