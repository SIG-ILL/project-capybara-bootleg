#pragma once

#include <memory>

#include "HeightMap.hpp"
#include "NoiseGenerator.hpp"

namespace pcb {
	class NoiseMapGenerator {
	public:
		NoiseMapGenerator(int width, int height);

		std::unique_ptr<Heightmap> generate(double noiseSamplingFrequencyX, double noiseSamplingFrequencyY, double xOffset, double yOffset) const;

	protected:
		virtual double generateElevationForNoiseCoordinates(double x, double y) const;
		double generateElevationFromNoiseValue(double noiseValue) const;

		NoiseGenerator noiseGenerator;

	private:
		int width;
		int height;
	};
}