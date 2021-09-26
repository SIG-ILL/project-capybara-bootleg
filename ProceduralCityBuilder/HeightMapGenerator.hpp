#pragma once

#include <vector>

#include "Heightmap.hpp"
#include "NoiseGenerator.hpp"

namespace pcb {
	class HeightmapGenerator final {
	public:
		HeightmapGenerator(int mapWidth, int mapHeight);

		Heightmap generate() const;

	private:
		int mapWidth;
		int mapHeight;
		NoiseGenerator noiseGenerator;

		void generateAndAddNoiseMap(std::vector<unsigned char>& noiseMap, double noiseModifier, double multiplier, double maxValueFactor) const;
		void generateAndSubtractNoiseMap(std::vector<unsigned char>& noiseMap, double noiseModifier, double multiplier, double maxValueFactor) const;
		double generateElevationForNoiseCoordinates(double x, double y) const;
	};
}