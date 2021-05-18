#pragma once

#include "HeightMap.hpp"
#include "NoiseGenerator.hpp"

namespace pcb {
	class HeightMapGenerator {
	public:
		HeightMapGenerator(int width, int height);

		pcb::HeightMap* generateNew();

	private:
		int width;
		int height;
		pcb::NoiseGenerator noiseGenerator;

		void createInitialLayer(unsigned char* noiseMap, double noiseModifier, double multiplier, double maxValueFactor);
		void addAdditiveLayer(unsigned char* noiseMap, double noiseModifier, double multiplier, double maxValueFactor);
		void addSubtractiveLayer(unsigned char* noiseMap, double noiseModifier, double multiplier, double maxValueFactor);
		double generateValueForCoordinates(double x, double y);
	};
}