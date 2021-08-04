#pragma once

#include "HeightMap.hpp"

namespace pcb {
	class MaskGenerator final {
	public:
		Heightmap generateCircleLinearFalloffMask(int width, int height, int unaffectedCircleRadiusInPixels, int falloffWidthInPixels, int offsetX, int offsetY) const;
		Heightmap generateRectangleLinearFalloffMask(int width, int height, int horizontalUnaffectedRadiusInPixels, int verticalUnaffectedRadiusInPixels, int falloffWidthInPixels, int offsetX, int offsetY) const;

	private:
		const unsigned char MIN_MASK_VALUE = 0;
		const unsigned char MAX_MASK_VALUE = 255;
	};
}