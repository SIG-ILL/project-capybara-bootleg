#include "AbsoluteNoiseMapGenerator.hpp"

pcb::AbsoluteNoiseMapGenerator::AbsoluteNoiseMapGenerator(int width, int height) : NoiseMapGenerator(width, height) {}

double pcb::AbsoluteNoiseMapGenerator::generateElevationForNoiseCoordinates(double x, double y) const {
	return generateElevationFromNoiseValue(std::abs(noiseGenerator.getValueForCoordinate(x, y)));
}