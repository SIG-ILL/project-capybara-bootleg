#pragma once

#include "ProceduralHeightmapOperation.hpp"

namespace pcb {
	class ProceduralHeightmapOperationEllipse final : public ProceduralHeightmapOperation {
	public:
		ProceduralHeightmapOperationEllipse(int width, int height, int unaffectedHorizontalSemiDiameterInPixels, int unaffectedVerticalSemiDiameterInPixels, float falloffEllipseScaleFactor, int offsetX, int offsetY);

		std::unique_ptr<Heightmap> generateResult() const override;
		int getHeightmapWidth() const override;
		int getHeightmapHeight() const override;

	private:
		int width;
		int height;
		int unaffectedHorizontalSemiDiameterInPixels;
		int unaffectedVerticalSemiDiameterInPixels;
		float falloffEllipseScaleFactor;
		int offsetX;
		int offsetY;
	};
}