#pragma once

#include <vector>

#include "Heightmap.hpp"
#include "NoiseGenerator.hpp"

namespace pcb {
	class HeightmapGenerator final {
	public:
		HeightmapGenerator(int mapWidth, int mapHeight);

		pcb::Heightmap* generateNew() const;

	private:
		int mapWidth;
		int mapHeight;
		pcb::NoiseGenerator noiseGenerator;

		void generateAndAddNoiseMap(unsigned char* noiseMap, double noiseModifier, double multiplier, double maxValueFactor) const;
		void generateAndSubtractNoiseMap(unsigned char* noiseMap, double noiseModifier, double multiplier, double maxValueFactor) const;
		double generateValueForCoordinates(double x, double y) const;
	};
}