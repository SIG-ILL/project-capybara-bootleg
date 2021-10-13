#include "RandomHeightmapLayerDataGenerator.hpp"

#include <array>

#include "RandomUniformInt.hpp"
#include "RandomUniformReal.hpp"
#include "RandomBinary.hpp"
#include "GradientDirection.hpp"

#pragma region Layer_Data
pcb::LayerData::LayerData(int width, int height, int layerIndex, LayerMode layerMode, const RandomGenerationControlParameters& properties) : layerIndex(layerIndex), layerMode(layerMode), width(width), height(height), generationParameters() {
	generateNoiseMapGenerationParameters(properties);
}

std::unique_ptr<pcb::LayerData> pcb::LayerData::getRandomAllowedNextNode(const std::vector<std::unique_ptr<LayerData>>& previousLayers, int nextLayerIndex, const RandomGenerationControlParameters& properties) const {
	const std::vector<LayerMode> allowedModes = determineAllowedNextModes(previousLayers);
	int amountOfAvailableModes = allowedModes.size();
	RandomUniformInt layerModeValues(0, amountOfAvailableModes - 1);
	int layerModeIndex = layerModeValues.generate();

	std::unique_ptr<LayerData> nextNode;
	switch (allowedModes.at(layerModeIndex)) {
	case LayerMode::Addition:
		nextNode = std::make_unique<AdditionLayerData>(width, height, nextLayerIndex, properties);
		break;
	case LayerMode::Subtraction:
		nextNode = std::make_unique<SubtractionLayerData>(width, height, nextLayerIndex, properties);
		break;
	case LayerMode::Mask:
		nextNode = std::make_unique<MaskLayerData>(width, height, nextLayerIndex, properties);
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

void pcb::LayerData::generateNoiseMapGenerationParameters(const RandomGenerationControlParameters& properties) {
	RandomBinary absoluteNoiseChance(properties.absoluteNoiseChance);
	generationParameters.isAbsolute = absoluteNoiseChance.generate();
	generationParameters.symmetricalFrequency = properties.layerBaseNoiseFrequency - (properties.layerNoiseFrequencyAdditionalLayerModifier * layerIndex);
	generationParameters.symmetricalFrequency *= generationParameters.isAbsolute ? properties.absoluteNoiseFrequencyModifier : 1;
	generationParameters.noiseOffsetX = generateNoiseOffset(properties);
	generationParameters.noiseOffsetY = generateNoiseOffset(properties);
	RandomBinary invertChance(generationParameters.isAbsolute ? properties.absoluteNoiseInversionChance : properties.defaultNoiseInversionChance);
	generationParameters.invert = invertChance.generate();
	RandomBinary maskChance(properties.applyMaskOnLayerChance);
	generationParameters.applyMask = maskChance.generate();

	if (generationParameters.applyMask) {
		RandomUniformInt maskLayerCountValues(properties.maskAmountOfLayersBounds);
		int amountOfLayers = maskLayerCountValues.generate();
		RandomBinary maskInvertChance(properties.maskInversionChance);

		RandomBinary binaryChance;
		RandomUniformReal offsetMultiplicationValues(properties.maskOffsetMultiplicationValueBounds);
		RandomUniformReal radiusMultiplicationValues(properties.maskRadiusMultiplicationValueBounds);
		RandomUniformReal falloffMultiplicationValues(properties.maskFalloffMultiplicationValueBounds);
		RandomUniformInt maskTypeValues(0, 3);
		RandomUniformInt gradientDirectionValues(0, 3);
		RandomUniformInt noiseFrequencyValues(1, 2);
		RandomBinary maskAbsoluteNoiseChance(properties.maskAbsoluteNoiseChance);
		RandomBinary absoluteNoiseInvertChance(properties.maskAbsoluteNoiseInversionChance);
		RandomBinary defaultNoiseInvertChance(properties.maskDefaultNoiseInversionChance);
		std::array<GradientDirection, 4> gradientDirections = { GradientDirection::Up, GradientDirection::Left, GradientDirection::Down, GradientDirection::Right };

		std::vector<CombinedMaskGenerationLayerParameters> layerParameters;
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
			bool generateAbsoluteNoise = maskAbsoluteNoiseChance.generate();
			int noiseFrequencyX = 128 * (noiseFrequencyValues.generate() / 2.0f);
			int noiseFrequencyY = 128 * (noiseFrequencyValues.generate() / 2.0f);
			if (generateAbsoluteNoise) {
				noiseFrequencyX *= properties.maskAbsoluteNoiseFrequencyModifier;
				noiseFrequencyY *= properties.maskAbsoluteNoiseFrequencyModifier;
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
				break;
			case 3:
				maskType = MaskType::Noise;
				break;
			}

			layerParameters.emplace_back(maskType, offsetX, offsetY, unaffectedRadiusX, unaffectedRadiusY, falloffWidth, gradientDirections[gradientDirectionIndex],
				noiseFrequencyX, noiseFrequencyY, generateAbsoluteNoise, invertLayer);
		}

		generationParameters.maskParameters = { layerParameters, maskInvertChance.generate() };
	}
}

int pcb::LayerData::generateNoiseOffset(const RandomGenerationControlParameters& properties) const {
	RandomUniformInt noiseOffsetValues(properties.noiseOffsetValueBounds);
	return noiseOffsetValues.generate() * noiseOffsetValues.generate();
}

pcb::AdditionLayerData::AdditionLayerData(int width, int height, int layerIndex, const RandomGenerationControlParameters& properties) : LayerData(width, height, layerIndex, LayerMode::Addition, properties) {}

std::vector<pcb::LayerMode> pcb::AdditionLayerData::determineAllowedNextModes(const std::vector<std::unique_ptr<LayerData>>& previousLayers) const {
	std::vector<LayerMode> allowedNextModes;
	if (previousLayers.back()->getMode() != LayerMode::Addition) {
		allowedNextModes.push_back(LayerMode::Addition);
	}

	allowedNextModes.push_back(LayerMode::Subtraction);
	allowedNextModes.push_back(LayerMode::Mask);

	return allowedNextModes;
}

pcb::SubtractionLayerData::SubtractionLayerData(int width, int height, int layerIndex, const RandomGenerationControlParameters& properties) : LayerData(width, height, layerIndex, LayerMode::Subtraction, properties) {}

std::vector<pcb::LayerMode> pcb::SubtractionLayerData::determineAllowedNextModes(const std::vector<std::unique_ptr<LayerData>>& previousLayers) const {
	std::vector<LayerMode> allowedNextModes;
	allowedNextModes.push_back(LayerMode::Addition);

	if (previousLayers.back()->getMode() == LayerMode::Addition) {
		allowedNextModes.push_back(LayerMode::Mask);
	}

	return allowedNextModes;
}

pcb::MaskLayerData::MaskLayerData(int width, int height, int layerIndex, const RandomGenerationControlParameters& properties) : LayerData(width, height, layerIndex, LayerMode::Mask, properties) {}

std::vector<pcb::LayerMode> pcb::MaskLayerData::determineAllowedNextModes(const std::vector<std::unique_ptr<LayerData>>& previousLayers) const {
	std::vector<LayerMode> allowedNextModes;
	allowedNextModes.push_back(LayerMode::Addition);

	if (previousLayers.back()->getMode() == LayerMode::Addition) {
		allowedNextModes.push_back(LayerMode::Subtraction);
	}

	return allowedNextModes;
}

pcb::FinalMaskLayerData::FinalMaskLayerData(int width, int height, int layerIndex, const RandomGenerationControlParameters& properties) : LayerData(width, height, layerIndex, LayerMode::FinalMask, properties) {}

std::vector<pcb::LayerMode> pcb::FinalMaskLayerData::determineAllowedNextModes(const std::vector<std::unique_ptr<LayerData>>& previousLayers) const {
	return std::vector<LayerMode>();
}
#pragma endregion

std::unique_ptr<std::vector<std::unique_ptr<pcb::LayerData>>> pcb::RandomHeightmapLayerDataGenerator::generateLayerData(int width, int height, int amountOfLayers, const RandomGenerationControlParameters& properties) const {
	std::unique_ptr<std::vector<std::unique_ptr<LayerData>>> layerData = std::make_unique<std::vector<std::unique_ptr<LayerData>>>();
	layerData->push_back(std::make_unique<AdditionLayerData>(width, height, 0, properties));
	for (int i = 0; i < (amountOfLayers - 1); i++) {
		layerData->push_back(layerData->back()->getRandomAllowedNextNode(*layerData, (i + 1), properties));
	}

	RandomBinary finalMaskChanceValues(properties.applyFinalMaskChance);
	bool applyFinalMask = finalMaskChanceValues.generate();
	if (applyFinalMask) {
		layerData->push_back(std::make_unique<FinalMaskLayerData>(width, height, layerData->size(), properties));
	}

	return layerData;
}