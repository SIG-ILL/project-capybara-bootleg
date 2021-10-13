#pragma once

#include <memory>

#include "Heightmap.hpp"
#include "HeightmapGenerationProperties.hpp"
#include "AbsoluteNoiseMapGenerator.hpp"

namespace pcb {
	class MaskGenerator final {
	public:
		MaskGenerator(int width, int height);

		std::unique_ptr<Heightmap> generateCircleLinearFalloffMask(int unaffectedCircleRadiusInPixels, int falloffWidthInPixels, int offsetX, int offsetY) const;
		std::unique_ptr<Heightmap> generateRectangleLinearFalloffMask(int horizontalUnaffectedRadiusInPixels, int verticalUnaffectedRadiusInPixels, int falloffWidthInPixels, int offsetX, int offsetY) const;
		std::unique_ptr<Heightmap> generateLinearGradientMask(GradientDirection direction) const;
		std::unique_ptr<Heightmap> generateCombinedMask(const CombinedMaskGenerationParameters& parameters) const;

	private:
		static const unsigned char MIN_MASK_VALUE = 0;
		static const unsigned char MAX_MASK_VALUE = 255;

		int width;
		int height;
		AbsoluteNoiseMapGenerator absoluteNoiseMapGenerator;
		NoiseMapGenerator noiseMapGenerator;
	};
}