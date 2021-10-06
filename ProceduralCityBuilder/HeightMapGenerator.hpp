#pragma once

#include <vector>

#include "Heightmap.hpp"
#include "NoiseGenerator.hpp"
#include "LayeredHeightmapGenerator.hpp"

namespace pcb {
	class HeightmapGenerator final : public LayeredHeightmapGenerator {
	public:
		HeightmapGenerator(int mapWidth, int mapHeight);

		std::unique_ptr<LayeredHeightmap> generate() const override;

	private:
		void generateAndAddNoiseMap(std::vector<unsigned char>& noiseMap, double noiseModifier, double multiplier, double maxValueFactor) const;
		void generateAndSubtractNoiseMap(std::vector<unsigned char>& noiseMap, double noiseModifier, double multiplier, double maxValueFactor) const;
	};
}