#pragma once

#include <memory>

#include "Heightmap.hpp"

namespace pcb {
	enum class GradientDirection { Up, Left, Down, Right };

	class MaskGenerator final {
	public:
		std::unique_ptr<Heightmap> generateCircleLinearFalloffMask(int width, int height, int unaffectedCircleRadiusInPixels, int falloffWidthInPixels, int offsetX, int offsetY) const;
		std::unique_ptr<Heightmap> generateRectangleLinearFalloffMask(int width, int height, int horizontalUnaffectedRadiusInPixels, int verticalUnaffectedRadiusInPixels, int falloffWidthInPixels, int offsetX, int offsetY) const;
		std::unique_ptr<Heightmap> generateLinearGradientMask(int width, int height, GradientDirection direction) const;

	private:
		static const unsigned char MIN_MASK_VALUE = 0;
		static const unsigned char MAX_MASK_VALUE = 255;
	};
}