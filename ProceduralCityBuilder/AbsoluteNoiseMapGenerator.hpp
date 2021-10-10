#pragma once

#include "NoiseMapGenerator.hpp"

namespace pcb {
	class AbsoluteNoiseMapGenerator final : public NoiseMapGenerator {
	public:
		AbsoluteNoiseMapGenerator(int width, int height);

	private:
		double generateElevationForNoiseCoordinates(double x, double y) const override;
	};
}