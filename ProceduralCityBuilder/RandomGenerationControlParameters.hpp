#pragma once

#include "BoundsPair.hpp"

namespace pcb {
	class RandomGenerationControlParametersRandom final {
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

	class RandomGenerationControlParameters final {
	public:
		double layerBaseNoiseFrequency;
		double layerNoiseFrequencyAdditionalLayerModifier;
		double layerScalingBaseValue;
		double layerScalingAdditionalLayerModifier;
		double absoluteNoiseFrequencyModifier;

		double maskAbsoluteNoiseFrequencyModifier;

		RandomGenerationControlParametersRandom randomControl;
	};
}