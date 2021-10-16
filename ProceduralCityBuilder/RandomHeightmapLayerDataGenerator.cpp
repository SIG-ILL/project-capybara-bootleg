#include "RandomHeightmapLayerDataGenerator.hpp"

#include <array>
#include <numbers>

#include "RandomUniformInt.hpp"
#include "RandomUniformReal.hpp"
#include "RandomBoolean.hpp"
#include "GradientDirection.hpp"
#include "Logger.hpp"

pcb::RandomHeightmapLayerDataGenerator::RandomHeightmapLayerDataGenerator() : RandomHeightmapLayerDataGenerator(getDefaultRandomGenerationControlParameters()) {}

pcb::RandomHeightmapLayerDataGenerator::RandomHeightmapLayerDataGenerator(const RandomGenerationControlParameters& controlParameters) : controlParameters(controlParameters) {}

std::unique_ptr<std::vector<std::unique_ptr<pcb::LayerData>>> pcb::RandomHeightmapLayerDataGenerator::generateLayerData(int width, int height) const {
	RandomUniformInt layerAmountValues(controlParameters.randomControl.amountOfLayersBounds);
	int amountOfLayers = layerAmountValues.generate();
	std::unique_ptr<std::vector<std::unique_ptr<LayerData>>> layerData = std::make_unique<std::vector<std::unique_ptr<LayerData>>>();
	layerData->push_back(std::make_unique<AdditionLayerData>(width, height, 0, controlParameters));	

	for (int i = 0; i < (amountOfLayers - 1); i++) {
		layerData->push_back(layerData->back()->getRandomAllowedNextNode(*layerData, (i + 1), controlParameters));
	}

	RandomBoolean finalMaskChanceValues(controlParameters.randomControl.applyFinalMaskChance);
	bool applyFinalMask = finalMaskChanceValues.generate();
	if (applyFinalMask) {
		layerData->push_back(std::make_unique<FinalMaskLayerData>(width, height, layerData->size(), controlParameters));
	}

	return layerData;
}

pcb::RandomGenerationControlParameters pcb::RandomHeightmapLayerDataGenerator::getDefaultRandomGenerationControlParameters() const {
	RandomGenerationControlParameters defaultParameters;

	defaultParameters.layerBaseNoiseFrequency = 128;
	defaultParameters.layerNoiseFrequencyAdditionalLayerModifier = 10;
	defaultParameters.layerScalingBaseValue = 0.5;
	defaultParameters.layerScalingAdditionalLayerModifier = 0.04;
	defaultParameters.absoluteNoiseFrequencyModifier = 2;

	defaultParameters.maskAbsoluteNoiseFrequencyModifier = 2;

	defaultParameters.randomControl.amountOfLayersBounds = { 1, 10 };

	defaultParameters.randomControl.noiseOffsetValueBounds = { 1, 10 };
	defaultParameters.randomControl.defaultNoiseInversionChance = 0.5;
	defaultParameters.randomControl.absoluteNoiseChance = 0.33;
	defaultParameters.randomControl.absoluteNoiseInversionChance = 0.5;

	defaultParameters.randomControl.applyMaskOnLayerChance = 0.33;
	defaultParameters.randomControl.maskAmountOfLayersBounds = { 1, 3 };
	defaultParameters.randomControl.maskOffsetMultiplicationValueBounds = { 0.0, 1.0 };
	defaultParameters.randomControl.maskRadiusMultiplicationValueBounds = { 0.25, 1.0 };
	defaultParameters.randomControl.maskFalloffMultiplicationValueBounds = { 0.1, 1.0 };
	defaultParameters.randomControl.maskDefaultNoiseInversionChance = 0.5;
	defaultParameters.randomControl.maskAbsoluteNoiseChance = 0.33;
	defaultParameters.randomControl.maskAbsoluteNoiseInversionChance = 0.5;
	defaultParameters.randomControl.maskInversionChance = 0.5;

	defaultParameters.randomControl.applyFinalMaskChance = 0.33;
	defaultParameters.randomControl.finalMaskAmountOfLayersBounds = { 2, 5 };
	defaultParameters.randomControl.finalMaskOffsetMultiplicationValueBounds = { 0.0, 1.0 };
	defaultParameters.randomControl.finalMaskRadiusMultiplicationValueBounds = { 0.25, 1.0 };
	defaultParameters.randomControl.finalMaskFalloffMultiplicationValueBounds = { 0.3, 1.0 };
	defaultParameters.randomControl.finalMaskCompositeMaskShapesDistanceMultiplierBounds = { 0.5, 1.0 };
	defaultParameters.randomControl.finalMaskInversionChance = 0.5;

	defaultParameters.randomControl.adjustmentScaleDownAmplitudeValueBounds = { 0.45, 0.75 };
	defaultParameters.randomControl.adjustmentScaleUpAmplitudeValueBounds = { 1.5, 2.2 };

	return defaultParameters;
}

void pcb::RandomHeightmapLayerDataGenerator::setRandomGenerationControlParameters(const RandomGenerationControlParameters& controlParameters) {
	this->controlParameters = controlParameters;
}