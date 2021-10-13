#pragma once

#include "BoundsPair.hpp"
#include "GradientDirection.hpp"

namespace pcb {
#pragma region Random_Generation_Control_Properties
	class RandomGenerationControlParameters final {
	public:
		BoundsPair<unsigned int> amountOfLayersBounds;

		BoundsPair<int> noiseOffsetValueBounds;
		double defaultNoiseInversionChance;
		double absoluteNoiseChance;
		double absoluteNoiseInversionChance;

		double applyMaskOnLayerChance;
		BoundsPair<unsigned int> maskAmountOfLayersBounds;
		BoundsPair<double> maskOffsetMultiplicationValueBounds;
		BoundsPair<double> maskRadiusMultiplicationValueBounds;
		BoundsPair<double> maskFalloffMultiplicationValueBounds;
		double maskDefaultNoiseInversionChance;
		double maskAbsoluteNoiseChance;
		double maskAbsoluteNoiseInversionChance;
		double maskInversionChance;

		double applyFinalMaskChance;
		BoundsPair<unsigned int> finalMaskAmountOfLayersBounds;
		BoundsPair<double> finalMaskOffsetMultiplicationValueBounds;
		BoundsPair<double> finalMaskRadiusMultiplicationValueBounds;
		BoundsPair<double> finalMaskFalloffMultiplicationValueBounds;
		BoundsPair<double> finalMaskCompositeMaskShapesDistanceMultiplierBounds;
		double finalMaskInversionChance;

		BoundsPair<double> adjustmentScaleDownAmplitudeValueBounds;
		BoundsPair<double> adjustmentScaleUpAmplitudeValueBounds;
	};
#pragma endregion

#pragma region Combined_Mask_Generation_Parameters
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
#pragma endregion

#pragma region Noise_Map_Generation_Parameters
	class NoiseMapGenerationParameters final {
	public:
		bool isAbsolute;
		double symmetricalFrequency;
		int noiseOffsetX;
		int noiseOffsetY;
		bool invert;
		bool applyMask;
		CombinedMaskGenerationParameters maskParameters;
	};
#pragma endregion

#pragma region Composite_Mask_Shape_Data
	class MaskShapeData final {
	public:
		MaskType maskType;
		int offsetX;
		int offsetY;
		int unaffectedRadiusX;
		int unaffectedRadiusY;
		int falloffWidth;
	};
#pragma endregion

#pragma region Final_Mask_Generation_Parameters
	class FinalMaskGenerationParameters final {
	public:
		std::vector<MaskShapeData> layerParameters;
		bool invert;
	};
#pragma endregion

	class RandomHeightmapGenerationGlobalParameters final {
	public:
		int amountOfLayers;

		BoundsPair<unsigned int> adjustmentLoweringThresholds;
		unsigned int adjustmentLoweringValue;
		unsigned int adjustmentScaleDownAmplitudeThreshold;
		double adjustmentScaleDownAmplitudeValue;
		unsigned int adjustmentScaleUpAmplitudeThreshold;
		double adjustmentScaleUpAmplitudeValue;
	};

	class RandomHeightmapGenerationLayerParameters final {
	public:
		double layerBaseNoiseFrequency;
		double layerNoiseFrequencyAdditionalLayerModifier;
		double layerScalingBaseValue;
		double layerScalingAdditionalLayerModifier;
		double absoluteNoiseFrequencyModifier;

		double maskAbsoluteNoiseFrequencyModifier;
	};

	class RandomHeightmapGenerationParameters final {
	public:
		RandomHeightmapGenerationGlobalParameters globalParameters;
		RandomHeightmapGenerationLayerParameters layerParameters;
	};
}