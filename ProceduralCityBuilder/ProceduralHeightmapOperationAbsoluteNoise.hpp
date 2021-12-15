#pragma once

#include "ProceduralHeightmapOperation.hpp"

namespace pcb {
	class ProceduralHeightmapOperationAbsoluteNoise : public ProceduralHeightmapOperation {
	public:
		ProceduralHeightmapOperationAbsoluteNoise(int width, int height, double noiseSamplingFrequencyX, double noiseSamplingFrequencyY, double xOffset, double yOffset);

		std::unique_ptr<Heightmap> generateResult() const override;

	private:
		int width;
		int height;
		double noiseSamplingFrequencyX;
		double noiseSamplingFrequencyY;
		double xOffset;
		double yOffset;
	};
}