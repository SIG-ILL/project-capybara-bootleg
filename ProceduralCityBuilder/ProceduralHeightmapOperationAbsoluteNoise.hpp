#pragma once

#include "ProceduralHeightmapOperation.hpp"

namespace pcb {
	class ProceduralHeightmapOperationAbsoluteNoise final : public ProceduralHeightmapOperation {
	public:
		ProceduralHeightmapOperationAbsoluteNoise(int width, int height, double noiseSamplingFrequencyX, double noiseSamplingFrequencyY, double xOffset, double yOffset);

		std::unique_ptr<Heightmap> generateResult() const override;
		int getHeightmapWidth() const override;
		int getHeightmapHeight() const override;

	private:
		int width;
		int height;
		double noiseSamplingFrequencyX;
		double noiseSamplingFrequencyY;
		double xOffset;
		double yOffset;
	};
}