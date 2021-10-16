#include "RandomHeightmapGenerationDataGenerator.hpp"

#include "RandomHeightmapLayerDataGenerator.hpp"
#include "RandomUniformReal.hpp"

pcb::RandomHeightmapGenerationData pcb::RandomHeightmapGenerationDataGenerator::generate(int mapWidth, int mapHeight) const {
	RandomHeightmapGenerationData returnValue;

	RandomHeightmapLayerDataGenerator layerDataGenerator;
	RandomGenerationControlParameters randomGenerationLayerControlParameters = layerDataGenerator.getDefaultRandomGenerationControlParameters();

	randomGenerationLayerControlParameters.layerBaseNoiseFrequency = 256;
	randomGenerationLayerControlParameters.layerNoiseFrequencyAdditionalLayerModifier = 20;
	layerDataGenerator.setRandomGenerationControlParameters(randomGenerationLayerControlParameters);

	RandomUniformReal scaleDownAmplitudeFactorValues(randomGenerationLayerControlParameters.randomControl.adjustmentScaleDownAmplitudeValueBounds);
	RandomUniformReal scaleUpAmplitudeFactorValues(randomGenerationLayerControlParameters.randomControl.adjustmentScaleUpAmplitudeValueBounds);

	returnValue.layerData = layerDataGenerator.generateLayerData(mapWidth, mapHeight);
	returnValue.adjustmentLoweringThresholds = { 50, 230 };
	returnValue.adjustmentLoweringValue = 30;
	returnValue.adjustmentScaleDownAmplitudeThreshold = 220;
	returnValue.adjustmentScaleDownAmplitudeValue = scaleDownAmplitudeFactorValues.generate();
	returnValue.adjustmentScaleUpAmplitudeThreshold = 100;
	returnValue.adjustmentScaleUpAmplitudeValue = scaleUpAmplitudeFactorValues.generate();

	return returnValue;
}