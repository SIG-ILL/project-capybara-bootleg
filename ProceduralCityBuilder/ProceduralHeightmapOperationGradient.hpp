#pragma once

#include "ProceduralHeightmapOperation.hpp"
#include "GradientDirection.hpp"

namespace pcb {
	class ProceduralHeightmapOperationGradient final : public ProceduralHeightmapOperation {
	public:
		ProceduralHeightmapOperationGradient(int width, int height, GradientDirection direction);

		std::unique_ptr<Heightmap> generateResult() const override;
		int getHeightmapWidth() const override;
		int getHeightmapHeight() const override;

	private:
		int width;
		int height;
		GradientDirection direction;

		// The different directions have different functions for performance/optimization reasons.
		void generateLinearGradientMaskUp(std::vector<unsigned char>& gradientData) const;
		void generateLinearGradientMaskDown(std::vector<unsigned char>& gradientData) const;
		void generateLinearGradientMaskLeft(std::vector<unsigned char>& gradientData) const;
		void generateLinearGradientMaskRight(std::vector<unsigned char>& gradientData) const;
	};
}