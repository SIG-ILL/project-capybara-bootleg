#include "ProceduralHeightmapOperationAbsoluteNoise.hpp"

#include "AbsoluteNoiseMapGenerator.hpp"

pcb::ProceduralHeightmapOperationAbsoluteNoise::ProceduralHeightmapOperationAbsoluteNoise(int width, int height, double noiseSamplingFrequencyX, double noiseSamplingFrequencyY, double xOffset, double yOffset)
	: width(width), height(height), noiseSamplingFrequencyX(noiseSamplingFrequencyX), noiseSamplingFrequencyY(noiseSamplingFrequencyY), xOffset(xOffset), yOffset(yOffset) {}

std::unique_ptr<pcb::Heightmap> pcb::ProceduralHeightmapOperationAbsoluteNoise::generateResult() const {
	AbsoluteNoiseMapGenerator generator(width, height);

	return generator.generate(noiseSamplingFrequencyX, noiseSamplingFrequencyY, xOffset, yOffset);
}

int pcb::ProceduralHeightmapOperationAbsoluteNoise::getHeightmapWidth() const {
	return width;
}

int pcb::ProceduralHeightmapOperationAbsoluteNoise::getHeightmapHeight() const {
	return height;
}