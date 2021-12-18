#pragma once

#include "ProceduralHeightmapOperation.hpp"

namespace pcb {
	class ProceduralHeightmapOperationRectangle final : public ProceduralHeightmapOperation {
	public:
		ProceduralHeightmapOperationRectangle(int width, int height, int horizontalUnaffectedRadiusInPixels, int verticalUnaffectedRadiusInPixels, int horizontalFalloffWidthInPixels, int verticalFalloffWidthInPixels, int offsetX, int offsetY);

		std::unique_ptr<Heightmap> generateResult() const override;
		int getHeightmapWidth() const override;
		int getHeightmapHeight() const override;

	private:
		int width;
		int height;
		int horizontalUnaffectedRadiusInPixels;
		int verticalUnaffectedRadiusInPixels;
		int horizontalFalloffWidthInPixels;
		int verticalFalloffWidthInPixels;
		int offsetX;
		int offsetY;
	};
}