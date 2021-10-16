#include "LayerData.hpp"

#include <numbers>

#include "RandomUniformInt.hpp"
#include "RandomUniformReal.hpp"
#include "RandomBoolean.hpp"
#include "Logger.hpp"

pcb::LayerData::LayerData(int width, int height, int layerIndex, LayerMode layerMode, const RandomGenerationControlParameters& controlParameters) : width(width), height(height), layerIndex(layerIndex), layerMode(layerMode), generationParameters() {
	generateNoiseMapGenerationParameters(controlParameters);
}

std::unique_ptr<pcb::LayerData> pcb::LayerData::getRandomAllowedNextNode(const std::vector<std::unique_ptr<LayerData>>& previousLayers, int nextLayerIndex, const RandomGenerationControlParameters& controlParameters) const {
	const std::vector<LayerMode> allowedModes = determineAllowedNextModes(previousLayers);
	int amountOfAvailableModes = allowedModes.size();
	RandomUniformInt layerModeValues(0, amountOfAvailableModes - 1);
	int layerModeIndex = layerModeValues.generate();

	std::unique_ptr<LayerData> nextNode;
	switch (allowedModes.at(layerModeIndex)) {
	case LayerMode::Addition:
		nextNode = std::make_unique<AdditionLayerData>(width, height, nextLayerIndex, controlParameters);
		break;
	case LayerMode::Subtraction:
		nextNode = std::make_unique<SubtractionLayerData>(width, height, nextLayerIndex, controlParameters);
		break;
	case LayerMode::Mask:
		nextNode = std::make_unique<MaskLayerData>(width, height, nextLayerIndex, controlParameters);
		break;
	}

	return nextNode;
}

int pcb::LayerData::getIndex() const {
	return layerIndex;
}

pcb::LayerMode pcb::LayerData::getMode() const {
	return layerMode;
}
pcb::NoiseMapGenerationParameters pcb::LayerData::getNoiseMapGenerationParameters() const {
	return generationParameters;
}

pcb::FinalMaskGenerationParameters pcb::LayerData::getFinalMaskGenerationParameters() const {
	return FinalMaskGenerationParameters();
}

int pcb::LayerData::countAmountOfConsecutiveLayerModesAtEnd(const std::vector<std::unique_ptr<LayerData>>& previousLayers, pcb::LayerMode layerMode) const {
	int count = 0;
	for (int i = previousLayers.size() - 1; i >= 0; i--) {
		if (previousLayers.at(i)->layerMode == layerMode) {
			count++;
		}
		else {
			break;
		}
	}

	return count;
}

void pcb::LayerData::generateNoiseMapGenerationParameters(const RandomGenerationControlParameters& controlParameters) {
	RandomBoolean absoluteNoiseChance(controlParameters.randomControl.absoluteNoiseChance);
	generationParameters.isAbsolute = absoluteNoiseChance.generate();
	generationParameters.symmetricalFrequency = controlParameters.layerBaseNoiseFrequency - (controlParameters.layerNoiseFrequencyAdditionalLayerModifier * layerIndex);
	generationParameters.symmetricalFrequency *= generationParameters.isAbsolute ? controlParameters.absoluteNoiseFrequencyModifier : 1;
	generationParameters.noiseOffsetX = generateNoiseOffset(controlParameters);
	generationParameters.noiseOffsetY = generateNoiseOffset(controlParameters);
	generationParameters.scalingFactorIfNotMask = controlParameters.layerScalingBaseValue - (layerIndex * controlParameters.layerScalingAdditionalLayerModifier);
	generationParameters.raiseAmountIfMask = 5 * layerIndex;
	RandomBoolean invertChance(generationParameters.isAbsolute ? controlParameters.randomControl.absoluteNoiseInversionChance : controlParameters.randomControl.defaultNoiseInversionChance);
	generationParameters.invert = invertChance.generate();
	RandomBoolean maskChance(controlParameters.randomControl.applyMaskOnLayerChance);
	generationParameters.applyMask = maskChance.generate();

	if (generationParameters.applyMask) {
		generationParameters.maskParameters = generateNoiseMapMaskParameters(controlParameters);
	}
}

int pcb::LayerData::generateNoiseOffset(const RandomGenerationControlParameters& controlParameters) const {
	RandomUniformInt noiseOffsetValues(controlParameters.randomControl.noiseOffsetValueBounds);
	return noiseOffsetValues.generate() * noiseOffsetValues.generate();
}

pcb::CombinedMaskGenerationParameters pcb::LayerData::generateNoiseMapMaskParameters(const RandomGenerationControlParameters& controlParameters) const {
	RandomUniformInt maskLayerCountValues(controlParameters.randomControl.maskAmountOfLayersBounds);
	int amountOfLayers = maskLayerCountValues.generate();
	RandomBoolean maskInvertChance(controlParameters.randomControl.maskInversionChance);

	RandomBoolean binaryChance;
	RandomUniformReal offsetMultiplicationValues(controlParameters.randomControl.maskOffsetMultiplicationValueBounds);
	RandomUniformReal radiusMultiplicationValues(controlParameters.randomControl.maskRadiusMultiplicationValueBounds);
	RandomUniformReal falloffMultiplicationValues(controlParameters.randomControl.maskFalloffMultiplicationValueBounds);
	RandomUniformInt maskTypeValues(0, 3);
	std::vector<GradientDirection> gradientDirections = { GradientDirection::Up, GradientDirection::Left, GradientDirection::Down, GradientDirection::Right };
	RandomUniformInt gradientDirectionValues(0, gradientDirections.size() - 1);
	RandomUniformInt noiseFrequencyValues(1, 2);
	RandomBoolean maskAbsoluteNoiseChance(controlParameters.randomControl.maskAbsoluteNoiseChance);
	RandomBoolean absoluteNoiseInvertChance(controlParameters.randomControl.maskAbsoluteNoiseInversionChance);
	RandomBoolean defaultNoiseInvertChance(controlParameters.randomControl.maskDefaultNoiseInversionChance);

	std::vector<CombinedMaskGenerationLayerParameters> maskLayerParameters;
	for (int i = 0; i < amountOfLayers; i++) {
		int offsetXModifier = binaryChance.generate() == true ? 1 : -1;
		int offsetYModifier = binaryChance.generate() == true ? 1 : -1;
		int offsetX = offsetMultiplicationValues.generate() * (offsetXModifier * 0.5f * width);
		int offsetY = offsetMultiplicationValues.generate() * (offsetYModifier * 0.5f * height);
		int unaffectedRadiusX = radiusMultiplicationValues.generate() * (0.25f * width);
		int unaffectedRadiusY = radiusMultiplicationValues.generate() * (0.25f * height);
		int falloffWidth = falloffMultiplicationValues.generate() * (0.25f * width);
		int maskTypeValue = maskTypeValues.generate();
		int gradientDirectionIndex = gradientDirectionValues.generate();
		GradientDirection gradientDirection = gradientDirections[gradientDirectionIndex];
		bool generateAbsoluteNoise = maskAbsoluteNoiseChance.generate();
		int noiseFrequencyX = 128 * (noiseFrequencyValues.generate() / 2.0f);
		int noiseFrequencyY = 128 * (noiseFrequencyValues.generate() / 2.0f);
		if (generateAbsoluteNoise) {
			noiseFrequencyX *= controlParameters.maskAbsoluteNoiseFrequencyModifier;
			noiseFrequencyY *= controlParameters.maskAbsoluteNoiseFrequencyModifier;
		}
		bool invertLayer = generateAbsoluteNoise ? absoluteNoiseInvertChance.generate() : defaultNoiseInvertChance.generate();

		MaskType maskType = MaskType::Circle;
		switch (maskTypeValue) {
		case 0:
			maskType = MaskType::Circle;
			break;
		case 1:
			maskType = MaskType::Rectangle;
			break;
		case 2:
			maskType = MaskType::LinearGradient;
			gradientDirections = removeOppositeGradientDirection(gradientDirection, gradientDirections);
			gradientDirectionValues = RandomUniformInt(0, gradientDirections.size() - 1);

			break;
		case 3:
			maskType = MaskType::Noise;
			break;
		}

		maskLayerParameters.emplace_back(maskType, offsetX, offsetY, unaffectedRadiusX, unaffectedRadiusY, falloffWidth, gradientDirection,
			noiseFrequencyX, noiseFrequencyY, generateAbsoluteNoise, invertLayer);
	}

	return { maskLayerParameters, maskInvertChance.generate() };
}

std::vector<GradientDirection> pcb::LayerData::removeOppositeGradientDirection(GradientDirection direction, const std::vector<GradientDirection>& directionsList) const {
	std::vector<GradientDirection> returnValue = directionsList;

	GradientDirection directionToRemove = GradientDirection::Down;
	if (direction == GradientDirection::Left) {
		directionToRemove = GradientDirection::Right;
	}
	else if (direction == GradientDirection::Down) {
		directionToRemove = GradientDirection::Up;
	}
	else if (direction == GradientDirection::Right) {
		directionToRemove = GradientDirection::Left;
	}

	for (int i = 0; i < returnValue.size(); i++) {
		if (returnValue[i] == directionToRemove) {
			returnValue.erase(returnValue.begin() + i);
			break;
		}
	}

	return returnValue;
}

pcb::AdditionLayerData::AdditionLayerData(int width, int height, int layerIndex, const RandomGenerationControlParameters& controlParameters) : LayerData(width, height, layerIndex, LayerMode::Addition, controlParameters) {}

std::vector<pcb::LayerMode> pcb::AdditionLayerData::determineAllowedNextModes(const std::vector<std::unique_ptr<LayerData>>& previousLayers) const {
	std::vector<LayerMode> allowedNextModes;
	if (previousLayers.back()->getMode() != LayerMode::Addition) {
		allowedNextModes.push_back(LayerMode::Addition);
	}

	allowedNextModes.push_back(LayerMode::Subtraction);
	allowedNextModes.push_back(LayerMode::Mask);

	return allowedNextModes;
}

pcb::SubtractionLayerData::SubtractionLayerData(int width, int height, int layerIndex, const RandomGenerationControlParameters& controlParameters) : LayerData(width, height, layerIndex, LayerMode::Subtraction, controlParameters) {}

std::vector<pcb::LayerMode> pcb::SubtractionLayerData::determineAllowedNextModes(const std::vector<std::unique_ptr<LayerData>>& previousLayers) const {
	std::vector<LayerMode> allowedNextModes;
	allowedNextModes.push_back(LayerMode::Addition);

	if (previousLayers.back()->getMode() == LayerMode::Addition) {
		allowedNextModes.push_back(LayerMode::Mask);
	}

	return allowedNextModes;
}

pcb::MaskLayerData::MaskLayerData(int width, int height, int layerIndex, const RandomGenerationControlParameters& controlParameters) : LayerData(width, height, layerIndex, LayerMode::Mask, controlParameters) {}

std::vector<pcb::LayerMode> pcb::MaskLayerData::determineAllowedNextModes(const std::vector<std::unique_ptr<LayerData>>& previousLayers) const {
	std::vector<LayerMode> allowedNextModes;
	allowedNextModes.push_back(LayerMode::Addition);

	if (previousLayers.back()->getMode() == LayerMode::Addition) {
		allowedNextModes.push_back(LayerMode::Subtraction);
	}

	return allowedNextModes;
}

pcb::FinalMaskLayerData::FinalMaskLayerData(int width, int height, int layerIndex, const RandomGenerationControlParameters& controlParameters) : LayerData(width, height, layerIndex, LayerMode::FinalMask, controlParameters), finalMaskGenerationParameters() {
	generateFinalMaskGenerationParameters(controlParameters);
}

pcb::FinalMaskGenerationParameters pcb::FinalMaskLayerData::getFinalMaskGenerationParameters() const {
	return finalMaskGenerationParameters;
}

std::vector<pcb::LayerMode> pcb::FinalMaskLayerData::determineAllowedNextModes(const std::vector<std::unique_ptr<LayerData>>& previousLayers) const {
	return std::vector<LayerMode>();
}

void pcb::FinalMaskLayerData::generateFinalMaskGenerationParameters(const RandomGenerationControlParameters& controlParameters) {
	Logger logger;
	logger << "---=== Final Mask Shape Data ===---\n";

	RandomUniformInt layerCountValues(controlParameters.randomControl.finalMaskAmountOfLayersBounds);
	int amountOfLayers = layerCountValues.generate();
	RandomBoolean invertChance(controlParameters.randomControl.finalMaskInversionChance);

	RandomBoolean binaryChance;
	RandomUniformReal offsetMultiplicationValues(controlParameters.randomControl.finalMaskOffsetMultiplicationValueBounds);
	RandomUniformReal radiusMultiplicationValues(controlParameters.randomControl.finalMaskRadiusMultiplicationValueBounds);
	RandomUniformReal falloffMultiplicationValues(controlParameters.randomControl.finalMaskFalloffMultiplicationValueBounds);

	std::vector<FinalMaskShapeData> layerParameters;
	for (int i = 0; i < amountOfLayers; i++) {
		MaskType maskType = binaryChance.generate() ? MaskType::Circle : MaskType::Rectangle;
		int offsetXModifier = binaryChance.generate() == true ? 1 : -1;
		int offsetYModifier = binaryChance.generate() == true ? 1 : -1;
		int offsetX = offsetMultiplicationValues.generate() * (offsetXModifier * 0.5f * width);
		int offsetY = offsetMultiplicationValues.generate() * (offsetYModifier * 0.5f * height);
		int unaffectedRadiusX = radiusMultiplicationValues.generate() * (0.25f * width);
		int unaffectedRadiusY = radiusMultiplicationValues.generate() * (0.25f * height);
		int falloffWidth = falloffMultiplicationValues.generate() * (0.25f * width);

		if (i > 0) {
			RandomUniformInt indexValues(0, i - 1);
			int previousShapeIndex = indexValues.generate();
			FinalMaskShapeData previousShape = layerParameters.at(previousShapeIndex);

			RandomUniformReal angleValues(0.0, 2 * std::numbers::pi);
			double angleFromPreviousShapeToNewShape = angleValues.generate();
			double cosAngle = std::cos(angleFromPreviousShapeToNewShape);
			double sinAngle = std::sin(angleFromPreviousShapeToNewShape);
			int minimumRadius = std::min(unaffectedRadiusX, unaffectedRadiusY);
			int maximumDistance = minimumRadius + falloffWidth + std::min(previousShape.unaffectedRadiusX, previousShape.unaffectedRadiusY);
			RandomUniformReal distanceMultiplierValues(controlParameters.randomControl.finalMaskCompositeMaskShapesDistanceMultiplierBounds);
			double distanceMultiplier = distanceMultiplierValues.generate();
			offsetX = (distanceMultiplier * cosAngle * maximumDistance) + previousShape.offsetX;
			offsetY = (distanceMultiplier * sinAngle * maximumDistance) + previousShape.offsetY;

			logger << "\n\n";
			logger << "PreviousShapeIndex: " << previousShapeIndex << "\n";
			logger << "AngleFromPreviousShapeToNewShape: " << angleFromPreviousShapeToNewShape * (180 / std::numbers::pi) << "\n";
			logger << "cosAngle: " << cosAngle << "\n";
			logger << "sinAngle: " << sinAngle << "\n";
			logger << "minimumRadius: " << minimumRadius << "\n";
			logger << "maximumDistance: " << maximumDistance << "\n";
			logger << "distanceMultiplier: " << distanceMultiplier << "\n";
			logger << "offsetFromPrevious: " << (distanceMultiplier * cosAngle * maximumDistance) << "; " << (distanceMultiplier * sinAngle * maximumDistance) << "\n";

			// TODO: Check if offsets are in map range!?
		}

		logger << "offset: " << offsetX << "; " << offsetY << "\n";
		logger << "RadiusX: " << unaffectedRadiusX << ", RadiusY: " << unaffectedRadiusY << "\n";
		logger << "Falloff: " << falloffWidth << "\n";

		layerParameters.emplace_back(maskType, offsetX, offsetY, unaffectedRadiusX, unaffectedRadiusY, falloffWidth);
	}

	logger << "---=== End of Final Mask Shape Data ===---\n\n";

	finalMaskGenerationParameters = { layerParameters, invertChance.generate() };
}