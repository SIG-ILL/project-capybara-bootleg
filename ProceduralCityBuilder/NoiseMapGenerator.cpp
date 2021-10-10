#include "NoiseMapGenerator.hpp"

#include <cmath>

pcb::NoiseMapGenerator::NoiseMapGenerator(int width, int height) : width(width), height(height), noiseGenerator() {}

std::unique_ptr<pcb::Heightmap> pcb::NoiseMapGenerator::generate(double noiseSamplingFrequencyX, double noiseSamplingFrequencyY, double xOffset, double yOffset) const {
	std::unique_ptr<std::vector<unsigned char>> noiseMap = std::make_unique<std::vector<unsigned char>>(width * height, 0);
	const double samplingDistanceX = 1.0 / noiseSamplingFrequencyX;
	const double samplingDistanceY = 1.0 / noiseSamplingFrequencyY;

	for (int y = 0; y < width; y++) {
		for (int x = 0; x < height; x++) {
			double noiseInputX = xOffset + (x * samplingDistanceX);
			double noiseInputY = yOffset + (y * samplingDistanceY);
			int index = (y * width) + x;
			(*noiseMap)[index] = static_cast<unsigned char>(generateElevationForNoiseCoordinates(noiseInputX, noiseInputY));
		}
	}

	return std::make_unique<Heightmap>(width, height, std::move(noiseMap));
}

double pcb::NoiseMapGenerator::generateElevationForNoiseCoordinates(double x, double y) const {
	return generateElevationFromNoiseValue(noiseGenerator.getValueForCoordinate(x, y));
}

double pcb::NoiseMapGenerator::generateElevationFromNoiseValue(double noiseValue) const {
	return std::round(127.5 * (1 + noiseValue));
}