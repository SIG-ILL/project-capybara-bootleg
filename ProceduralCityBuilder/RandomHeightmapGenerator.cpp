#include "RandomHeightmapGenerator.hpp"

#include <numbers>

#include "MaskGenerator.hpp"
#include "RandomUniformInt.hpp"
#include "RandomUniformReal.hpp"
#include "RandomBinary.hpp"

#include <iostream>

pcb::RandomHeightmapGenerator::RandomHeightmapGenerator(int mapWidth, int mapHeight) : mapWidth(mapWidth), mapHeight(mapHeight), noiseGenerator() {}

// TODO: Rotation of layers (lack of rotation is especially noticeable with 'stretched' noise).
pcb::LayeredHeightmap* pcb::RandomHeightmapGenerator::generateNew() const {
	RandomUniformInt layerAmountValues(1, 10);
	int numberOfLayers = layerAmountValues.generate();
	std::vector<LayerMode> layerModes = generateLayerModes(numberOfLayers);

	RandomUniformInt noiseOffsetValues(1, 10);
	RandomUniformInt maskChanceValues(0, 2);
	pcb::LayeredHeightmap* heightmap = new LayeredHeightmap(mapWidth, mapHeight);
	std::vector<HeightmapLayer> layers;

	for (int i = 0; i < layerModes.size(); i++) {
		int frequencyX = 128 - (10 * i);
		int frequencyY = 128 - (10 * i);
		int noiseOffsetX = noiseOffsetValues.generate() * noiseOffsetValues.generate();
		int noiseOffsetY = noiseOffsetValues.generate() * noiseOffsetValues.generate();

		switch (layerModes.at(i)) {
		case LayerMode::Addition:
			layers.emplace_back(generateHeightmap(frequencyX, frequencyY, noiseOffsetX, noiseOffsetY), layerModes.at(i));
			layers.back().heightmap.scale(0.5f - (i * 0.04));
			break;
		case LayerMode::Subtraction:
			layers.emplace_back(generateHeightmap(frequencyX, frequencyY, noiseOffsetX, noiseOffsetY), layerModes.at(i));
			layers.back().heightmap.scale(0.5f - (i * 0.04));
			break;
		case LayerMode::Mask:
			layers.emplace_back(generateHeightmap(frequencyX, frequencyY, noiseOffsetX, noiseOffsetY), layerModes.at(i));
			layers.back().heightmap.raise(i * 5);
			break;
		}

		std::cout << "Layer index " << i << "\n";

		if (maskChanceValues.generate() == 0) {
			Heightmap mask = generateMask();
			layers.back().heightmap.mask(mask);
		}
	}

	RandomUniformInt finalMaskChanceValues(0, 2);
	if (/*finalMaskChanceValues.generate() == 0*/ true) {
		Heightmap finalMask = generateFinalMask();
		layers.emplace_back(finalMask, LayerMode::Mask);
	}

	for (const HeightmapLayer& layer : layers) {
		heightmap->addLayer(layer.heightmap, layer.mode);
	}

	if (heightmap->getLowestElevation() > 50 && heightmap->getHighestElevation() > 230) {
		heightmap->lower(30);
	}

	if (heightmap->getHighestElevation() - heightmap->getLowestElevation() > 220) {
		RandomUniformReal scaleAmplitudeFactorValues(0.45, 0.75);
		heightmap->scaleAmplitude(scaleAmplitudeFactorValues.generate());
	}
	else if (heightmap->getHighestElevation() - heightmap->getLowestElevation() < 100) {
		RandomUniformReal scaleAmplitudeFactorValues(1.5, 2.2);
		heightmap->scaleAmplitude(scaleAmplitudeFactorValues.generate());
	}

	return heightmap;
}

std::vector<pcb::LayerMode> pcb::RandomHeightmapGenerator::generateLayerModes(const int numberOfLayers) const {
	std::vector<LayerData*> layerData;
	layerData.emplace_back(new AdditionLayerData());
	for (int i = 0; i < numberOfLayers; i++) {
		const std::vector<LayerMode> allowedModes = layerData.back()->getAllowedNextModes(layerData);
		int amountOfAvailableModes = allowedModes.size();
		RandomUniformInt layerModeValues(0, amountOfAvailableModes - 1);
		int layerModeIndex = layerModeValues.generate();
		LayerMode layerMode = allowedModes.at(layerModeIndex);

		switch (layerMode) {
			case LayerMode::Addition:
				layerData.emplace_back(new AdditionLayerData());
				break;
			case LayerMode::Subtraction:
				layerData.emplace_back(new SubtractionLayerData());
				break;
			case LayerMode::Mask:
				layerData.emplace_back(new MaskLayerData());
				break;
		}
	}

	std::vector<LayerMode> layerModes;
	for (const LayerData* layerDataElement : layerData) {
		layerModes.emplace_back(layerDataElement->getMode());
	}

	for (int i = 0; i < layerData.size(); i++) {
		delete layerData.at(i);
	}
	layerData.clear();

	return layerModes;
}

pcb::Heightmap pcb::RandomHeightmapGenerator::generateHeightmap(double noiseSamplingFrequencyX, double noiseSamplingFrequencyY, double xOffset, double yOffset) const {
	unsigned char* noiseMap = new unsigned char[mapWidth * mapHeight];
	const double samplingFrequencyToSamplingDistanceModifierX = 1.0 / noiseSamplingFrequencyX;
	const double samplingFrequencyToSamplingDistanceModifierY = 1.0 / noiseSamplingFrequencyY;

	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			double noiseInputX = xOffset + (x * samplingFrequencyToSamplingDistanceModifierX);
			double noiseInputY = yOffset + (y * samplingFrequencyToSamplingDistanceModifierY);
			noiseMap[(y * mapWidth) + x] = static_cast<unsigned char>(generateElevationForNoiseCoordinates(noiseInputX, noiseInputY));
		}
	}

	pcb::Heightmap heightmap(mapWidth, mapHeight, noiseMap);
	delete[] noiseMap;

	return heightmap;
}

double pcb::RandomHeightmapGenerator::generateElevationForNoiseCoordinates(double x, double y) const {
	return std::round(127.5 * (1 + noiseGenerator.getValueForCoordinates(x, y)));
}

pcb::Heightmap pcb::RandomHeightmapGenerator::generateMask() const {
	MaskGenerator maskGenerator;
	RandomBinary binaryChance;
	RandomUniformReal offsetMultiplicationValues(0.0, 1.0);
	RandomUniformReal radiusMultiplicationValues(0.25, 1.0);
	RandomUniformReal falloffMultiplicationValues(0.1, 1.0);
	std::vector<Heightmap> maskLayers;
	RandomUniformInt layerCountValues(1, 3);
	int amountOfLayers = layerCountValues.generate();
	RandomUniformInt maskTypeValues(0, 2);

	std::cout << "Amount of mask layers: " << amountOfLayers << "\n";

	for (int i = 0; i < amountOfLayers; i++) {
		int offsetXModifier = binaryChance.generate() == true ? 1 : -1;
		int offsetYModifier = binaryChance.generate() == true ? 1 : -1;
		int offsetX = offsetMultiplicationValues.generate() * (offsetXModifier * 0.5f * mapWidth);
		int offsetY = offsetMultiplicationValues.generate() * (offsetYModifier * 0.5f * mapHeight);
		int unaffectedRadiusX = radiusMultiplicationValues.generate() * (0.25f * mapWidth);
		int unaffectedRadiusY = radiusMultiplicationValues.generate() * (0.25f * mapHeight);
		int falloffWidth = falloffMultiplicationValues.generate() * (0.25f * mapWidth);

		int maskTypeValue = maskTypeValues.generate();
		std::cout << "MaskTypeValue: " << maskTypeValue << "\n";
		if (maskTypeValue == 0) {
			maskLayers.emplace_back(maskGenerator.generateCircleLinearFalloffMask(mapWidth, mapHeight, unaffectedRadiusX, falloffWidth, offsetX, offsetY));
		}
		else if(maskTypeValue == 1) {
			maskLayers.emplace_back(maskGenerator.generateRectangleLinearFalloffMask(mapWidth, mapHeight, unaffectedRadiusX, unaffectedRadiusY, falloffWidth, offsetX, offsetY));
		}
		else {
			RandomUniformInt noiseFrequencyValues(1, 2);
			int frequencyX = 128 * (noiseFrequencyValues.generate() / 2.0f);
			int frequencyY = 128 * (noiseFrequencyValues.generate() / 2.0f);
			maskLayers.emplace_back(generateHeightmap(frequencyX, frequencyY, offsetX, offsetY));
		}		
	}

	Heightmap mask = maskLayers.front();
	for (int i = 1; i < maskLayers.size(); i++) {
		mask.add(maskLayers.at(i));
	}

	if (binaryChance.generate() == true) {
		std::cout << "INVERTED!" << "\n";
		mask.invert();
	}

	std::cout << "\n";

	return mask;
}

pcb::Heightmap pcb::RandomHeightmapGenerator::generateFinalMask() const {
	MaskGenerator maskGenerator;
	RandomBinary binaryChance;
	RandomUniformReal offsetMultiplicationValues(0.0, 1.0);
	RandomUniformReal radiusMultiplicationValues(0.25, 1.0);
	RandomUniformReal falloffMultiplicationValues(0.3, 1.0);
	std::vector<Heightmap> maskLayers;
	RandomUniformInt layerCountValues(/*1, 3*/ 2, 3);
	int amountOfLayers = layerCountValues.generate();
	RandomBinary maskTypeValues;

	std::vector<MaskShapeData> maskShapeData;

	std::cout << "\n\n";

	for (int i = 0; i < amountOfLayers; i++) {
		int offsetXModifier = binaryChance.generate() == true ? 1 : -1;
		int offsetYModifier = binaryChance.generate() == true ? 1 : -1;
		int offsetX = offsetMultiplicationValues.generate() * (offsetXModifier * 0.5f * mapWidth);
		int offsetY = offsetMultiplicationValues.generate() * (offsetYModifier * 0.5f * mapHeight);
		int unaffectedRadiusX = radiusMultiplicationValues.generate() * (0.25f * mapWidth);
		int unaffectedRadiusY = radiusMultiplicationValues.generate() * (0.25f * mapHeight);
		int falloffWidth = falloffMultiplicationValues.generate() * (0.25f * mapWidth);

		if (maskShapeData.size() > 0) {
			RandomUniformInt indexValues(0, maskShapeData.size() - 1);
			int previousShapeIndex = indexValues.generate();
			MaskShapeData previousShape = maskShapeData.at(previousShapeIndex);

			RandomUniformReal angleValues(0.0, 2 * std::numbers::pi);
			double angleFromPreviousShapeToNewShape = angleValues.generate();
			double cosAngle = std::cos(angleFromPreviousShapeToNewShape);
			double sinAngle = std::sin(angleFromPreviousShapeToNewShape);

			int minimumRadius = std::min(unaffectedRadiusX, unaffectedRadiusY);
			int maximumDistance = minimumRadius + falloffWidth + std::min(previousShape.unaffectedRadiusX, previousShape.unaffectedRadiusY);
			RandomUniformReal distanceMultiplierValues(0.5, 1.0);
			double distanceMultiplier = distanceMultiplierValues.generate();
			offsetX = (distanceMultiplier * cosAngle * maximumDistance) + previousShape.offsetX;
			offsetY = (distanceMultiplier * sinAngle * maximumDistance) + previousShape.offsetY;

			std::cout << "\n\n";
			std::cout << "PreviousShapeIndex: " << previousShapeIndex << "\n";
			std::cout << "AngleFromPreviousShapeToNewShape: " << angleFromPreviousShapeToNewShape * (180 / std::numbers::pi) << "\n";
			std::cout << "cosAngle: " << cosAngle << "\n";
			std::cout << "sinAngle: " << sinAngle << "\n";
			std::cout << "minimumRadius: " << minimumRadius << "\n";
			std::cout << "maximumDistance: " << maximumDistance << "\n";
			std::cout << "distanceMultiplier: " << distanceMultiplier << "\n";
			std::cout << "offsetFromPrevious: " << (distanceMultiplier * cosAngle * maximumDistance) << "; " << (distanceMultiplier * sinAngle * maximumDistance) << "\n";
			std::cout << "offset: " << offsetX << "; " << offsetY << "\n";

			// TODO: Check if offsets are in map range!
		}
		else {
			std::cout << "offset: " << offsetX << "; " << offsetY << "\n";
			std::cout << "RadiusX: " << unaffectedRadiusX << ", RadiusY: " << unaffectedRadiusY << "\n";
			std::cout << "Falloff: " << falloffWidth << "\n";
		}

		if (maskTypeValues.generate() == true) {
			maskLayers.emplace_back(maskGenerator.generateCircleLinearFalloffMask(mapWidth, mapHeight, unaffectedRadiusX, falloffWidth, offsetX, offsetY));
			std::cout << "CIRCLE\n";
		}
		else {
			maskLayers.emplace_back(maskGenerator.generateRectangleLinearFalloffMask(mapWidth, mapHeight, unaffectedRadiusX, unaffectedRadiusY, falloffWidth, offsetX, offsetY));
			std::cout << "RECTANGLE\n";
		}

		maskShapeData.push_back({offsetX, offsetY, unaffectedRadiusX, unaffectedRadiusY});
	}

	Heightmap mask = maskLayers.front();
	for (int i = 1; i < maskLayers.size(); i++) {
		mask.add(maskLayers.at(i));
	}

	if (binaryChance.generate() == true) {
		mask.invert();
	}

	return mask;
}

#pragma region Layer_Data
pcb::RandomHeightmapGenerator::LayerData::LayerData(const LayerMode layerMode) : layerMode(layerMode) {}

const std::vector<pcb::LayerMode> pcb::RandomHeightmapGenerator::LayerData::getAllowedNextModes(const std::vector<LayerData*> previousLayers) const {
	return determineAllowedNextModes(previousLayers);
}

const pcb::LayerMode pcb::RandomHeightmapGenerator::LayerData::getMode() const {
	return layerMode;
}

int pcb::RandomHeightmapGenerator::LayerData::countAmountOfConsecutiveLayerModesAtEnd(const std::vector<LayerData*> previousLayers, pcb::LayerMode layerMode) const {
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

pcb::RandomHeightmapGenerator::AdditionLayerData::AdditionLayerData() : LayerData(LayerMode::Addition) {}

std::vector<pcb::LayerMode> pcb::RandomHeightmapGenerator::AdditionLayerData::determineAllowedNextModes(const std::vector<LayerData*> previousLayers) const {
	std::vector<LayerMode> allowedNextModes;
	if (previousLayers.back()->getMode() != LayerMode::Addition) {
		allowedNextModes.emplace_back(LayerMode::Addition);
	}

	allowedNextModes.emplace_back(LayerMode::Subtraction);
	allowedNextModes.emplace_back(LayerMode::Mask);

	return allowedNextModes;
}

pcb::RandomHeightmapGenerator::SubtractionLayerData::SubtractionLayerData() : LayerData(LayerMode::Subtraction) {}

std::vector<pcb::LayerMode> pcb::RandomHeightmapGenerator::SubtractionLayerData::determineAllowedNextModes(const std::vector<LayerData*> previousLayers) const {
	std::vector<LayerMode> allowedNextModes;
	allowedNextModes.emplace_back(LayerMode::Addition);

	if (previousLayers.back()->getMode() == LayerMode::Addition) {
		allowedNextModes.emplace_back(LayerMode::Mask);
	}	

	return allowedNextModes;
}

pcb::RandomHeightmapGenerator::MaskLayerData::MaskLayerData() : LayerData(LayerMode::Mask) {}

std::vector<pcb::LayerMode> pcb::RandomHeightmapGenerator::MaskLayerData::determineAllowedNextModes(std::vector<LayerData*> previousLayers) const {
	std::vector<LayerMode> allowedNextModes;
	allowedNextModes.emplace_back(LayerMode::Addition);

	if (previousLayers.back()->getMode() == LayerMode::Addition) {
		allowedNextModes.emplace_back(LayerMode::Subtraction);
	}	

	return allowedNextModes;
}
#pragma endregion