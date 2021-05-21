#pragma once

#include <vector>

#include "LayeredHeightmap.hpp"
#include "NoiseGenerator.hpp"

namespace pcb {
	class LayeredHeightmapGenerator final {
	public:
		LayeredHeightmapGenerator(int mapWidth, int mapHeight);

		pcb::LayeredHeightmap* generateNew() const;

	private:
		int mapWidth;
		int mapHeight;
		pcb::NoiseGenerator noiseGenerator;

		pcb::Heightmap generateHeightmap(double noiseModifier, double multiplier, double maxValueFactor) const;
		double generateValueForCoordinates(double x, double y) const;
	};
}