#include "ProceduralHeightmapOperationNoise.hpp"

#include "NoiseMapGenerator.hpp"

pcb::ProceduralHeightmapOperationNoise::ProceduralHeightmapOperationNoise(int width, int height, double noiseSamplingFrequencyX, double noiseSamplingFrequencyY, double xOffset, double yOffset)
	: width(width), height(height), noiseSamplingFrequencyX(noiseSamplingFrequencyX), noiseSamplingFrequencyY(noiseSamplingFrequencyY), xOffset(xOffset), yOffset(yOffset) {}

std::unique_ptr<pcb::Heightmap> pcb::ProceduralHeightmapOperationNoise::generateResult() const {
	NoiseMapGenerator generator(width, height);

	return generator.generate(noiseSamplingFrequencyX, noiseSamplingFrequencyY, xOffset, yOffset);
}