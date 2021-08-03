#pragma once

#include "HeightMap.hpp"

namespace pcb {
	class MaskGenerator final {
	public:
		Heightmap generateCircleLinearFalloffMask(int width, int height, int unaffectedCircleRadiusInPixels, int falloffWidthInPixels);
		Heightmap generateRectangleLinearFalloffMask(int width, int height, int horizontalUnaffectedSizeInPixels, int verticalUnaffectedSizeInPixels, int falloffWidthInPixels);

	private:
	};
}