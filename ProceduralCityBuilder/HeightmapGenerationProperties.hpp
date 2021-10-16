#pragma once

#include "GradientDirection.hpp"

namespace pcb {
	enum class MaskType { Circle, Rectangle, LinearGradient, Noise };

	class CombinedMaskGenerationLayerParameters final {
	public:
		MaskType maskType;
		int offsetX;
		int offsetY;
		int unaffectedRadiusX;
		int unaffectedRadiusY;
		int falloffWidth;
		GradientDirection gradientDirection;
		int noiseFrequencyX;
		int noiseFrequencyY;
		bool isAbsoluteNoise;
		bool invert;
	};

	class CombinedMaskGenerationParameters final {
	public:
		std::vector<CombinedMaskGenerationLayerParameters> layerParameters;
		bool invert;
	};

	class NoiseMapGenerationParameters final {
	public:
		bool isAbsolute;
		double symmetricalFrequency;
		int noiseOffsetX;
		int noiseOffsetY;
		double scalingFactorIfNotMask;
		unsigned char raiseAmountIfMask;
		bool invert;
		bool applyMask;
		CombinedMaskGenerationParameters maskParameters;
	};

	class FinalMaskShapeData final {
	public:
		MaskType maskType;
		int offsetX;
		int offsetY;
		int unaffectedRadiusX;
		int unaffectedRadiusY;
		int falloffWidth;
	};

	class FinalMaskGenerationParameters final {
	public:
		std::vector<FinalMaskShapeData> layerParameters;
		bool invert;
	};
}