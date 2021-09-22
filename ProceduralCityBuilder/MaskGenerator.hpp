#pragma once

#include "HeightMap.hpp"

namespace pcb {
	enum class GradientDirection { Up, Left, Down, Right };

	class MaskGenerator final {
	public:
		Heightmap generateCircleLinearFalloffMask(int width, int height, int unaffectedCircleRadiusInPixels, int falloffWidthInPixels, int offsetX, int offsetY) const;
		Heightmap generateRectangleLinearFalloffMask(int width, int height, int horizontalUnaffectedRadiusInPixels, int verticalUnaffectedRadiusInPixels, int falloffWidthInPixels, int offsetX, int offsetY) const;
		Heightmap generateLinearGradientMask(int width, int height, GradientDirection direction) const;

	private:
		static const unsigned char MIN_MASK_VALUE = 0;
		static const unsigned char MAX_MASK_VALUE = 255;
	};
}