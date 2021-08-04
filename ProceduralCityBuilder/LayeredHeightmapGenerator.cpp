#include "LayeredHeightmapGenerator.hpp"

#include <cmath>
#include <random>

#include "MaskGenerator.hpp"

pcb::LayeredHeightmapGenerator::LayeredHeightmapGenerator(int mapWidth, int mapHeight) : mapWidth(mapWidth), mapHeight(mapHeight), noiseGenerator() {}

pcb::LayeredHeightmap* pcb::LayeredHeightmapGenerator::generateNew() const {
	std::vector<Heightmap> heightmapLayers;
	MaskGenerator maskGenerator;
	heightmapLayers.emplace_back(maskGenerator.generateCircleLinearFalloffMask(mapWidth, mapHeight, (mapWidth/256.0f) * 30, (mapWidth / 256.0f) * 98, 0, 0));

	heightmapLayers.emplace_back(generateHeightmap(128, 128, 0, 0));
	heightmapLayers.back().lowerToLevel(175);
	heightmapLayers.back().scale(0.5);
	LayeredHeightmap* heightmap = new LayeredHeightmap(heightmapLayers.back());

	heightmapLayers.emplace_back(generateHeightmap(1024, 128, 8, 0));
	heightmapLayers.back().scale(0.5);
	heightmapLayers.back().scaleAmplitude(2);	
	heightmapLayers.back().mask(heightmapLayers.at(0));
	heightmap->addLayer(heightmapLayers.back(), LayerMode::Addition);

	heightmapLayers.emplace_back(generateHeightmap(32, 32, 0, 8));
	heightmapLayers.back().scale(0.0625);
	heightmap->addLayer(heightmapLayers.back(), LayerMode::Addition);

	heightmapLayers.emplace_back(generateHeightmap(16, 16, 8, 8));
	heightmapLayers.back().scale(0.03125);
	heightmap->addLayer(heightmapLayers.back(), LayerMode::Addition);

	heightmap->scale(1.5);
	heightmap->addLayer(heightmapLayers.at(0), LayerMode::Mask);
	heightmapLayers.emplace_back(maskGenerator.generateRectangleLinearFalloffMask(mapWidth, mapHeight, (mapWidth / 256.0f) * 15, (mapHeight / 256.0f) * 15, (mapWidth / 256.0f) * 64, 0, 0));
	heightmapLayers.back().invert();
	heightmap->addLayer(heightmapLayers.back(), LayerMode::Mask);

	heightmapLayers.emplace_back(generateHeightmap(16, 16, 0, 0));
	heightmapLayers.back().mask(*heightmap);
	heightmapLayers.at(0).invert();
	heightmapLayers.back().mask(heightmapLayers.at(0));
	heightmap->addLayer(heightmapLayers.back(), LayerMode::Addition);
	heightmapLayers.at(3).scale(10);
	heightmap->addLayer(heightmapLayers.at(3), LayerMode::Mask);

	return heightmap;
}

pcb::LayeredHeightmap* pcb::LayeredHeightmapGenerator::generateNewRandom() const {
	std::random_device randomDevice;
	std::mt19937 randomGenerator(randomDevice());
	std::uniform_int_distribution<> distribution(1, 10);
	std::uniform_int_distribution<> halfDistribution(1, 5);

	int numberOfLayers = distribution(randomGenerator);

	std::uniform_int_distribution<> binaryDistribution(0, 1);
	std::uniform_int_distribution<> maskChanceDistribution(0, 2);
	std::uniform_real_distribution<> multiplicationDistribution(0.0, 1.0);

	MaskGenerator maskGenerator;
	std::vector<Heightmap> heightmapLayers;
	for (int i = 0; i < numberOfLayers; i++) {
		int frequencyX = 12 * distribution(randomGenerator);
		int frequencyY = 12 * distribution(randomGenerator);
		heightmapLayers.emplace_back(generateHeightmap(frequencyX, frequencyY, distribution(randomGenerator) * distribution(randomGenerator), distribution(randomGenerator) * distribution(randomGenerator)));
		heightmapLayers.back().scale(0.5f - (i * 0.03));	// This isn't useful for masks (in fact it's bothersome as 'later' masks will nullify any work done before). This shows that we need to determine the layer types beforehand, and apply rules to them based on their position in the stack.

		int choice = binaryDistribution(randomGenerator);
		choice = maskChanceDistribution(randomGenerator);
		if (choice == 0) {
			choice = binaryDistribution(randomGenerator);
			if (choice == 0) {
				heightmapLayers.emplace_back(maskGenerator.generateCircleLinearFalloffMask(mapWidth, mapHeight, multiplicationDistribution(randomGenerator) * (0.25f * mapWidth), multiplicationDistribution(randomGenerator) * (0.25f * mapWidth), multiplicationDistribution(randomGenerator) * (0.66f * mapWidth), multiplicationDistribution(randomGenerator) * (0.66f * mapHeight)));
			}
			else {
				heightmapLayers.emplace_back(maskGenerator.generateRectangleLinearFalloffMask(mapWidth, mapHeight, multiplicationDistribution(randomGenerator) * (0.25f * mapWidth), multiplicationDistribution(randomGenerator) * (0.25f * mapHeight), multiplicationDistribution(randomGenerator) * (0.25f * mapWidth), multiplicationDistribution(randomGenerator) * (0.66f * mapWidth), multiplicationDistribution(randomGenerator) * (0.66f * mapHeight)));
			}

			choice = maskChanceDistribution(randomGenerator);
			if (choice == 0) {
				heightmapLayers.back().invert();
			}

			heightmapLayers.at(heightmapLayers.size() - 2).mask(heightmapLayers.back());
			heightmapLayers.pop_back();
		}
	}

	pcb::LayeredHeightmap* heightmap = new LayeredHeightmap(generateHeightmap(128, 128, distribution(randomGenerator) * distribution(randomGenerator), distribution(randomGenerator) * distribution(randomGenerator)));
	std::uniform_int_distribution<> layerModeCorrectionDistribution(7, 10);
	for (int i = 0, additionCounter = 0, additionLimit = 2, subtractionCounter = 0, maskCounter = 0; i < heightmapLayers.size(); i++) {
		int selectionValue = distribution(randomGenerator);
		if (additionCounter == additionLimit) {
			additionCounter = 0;
			additionLimit++;
			selectionValue = layerModeCorrectionDistribution(randomGenerator);
		}
		if (subtractionCounter == 1) {
			subtractionCounter = 0;
			if (binaryDistribution(randomGenerator) == 0) {
				selectionValue = 1;
			}
			else {
				selectionValue = 10;
			}
		}
		if (maskCounter == 1) {
			maskCounter = 0;
			selectionValue = 1;
		}
		if (i == heightmapLayers.size() - 1 && (selectionValue == 7 || selectionValue == 8)) {
			if (binaryDistribution(randomGenerator) == 0) {
				selectionValue = 1;
			}
			else {
				selectionValue = 10;
			}
		}

		LayerMode layerMode;
		if (selectionValue == 7 || selectionValue == 8) {
			layerMode = LayerMode::Subtraction;
			subtractionCounter++;
		}
		else if(selectionValue == 9 || selectionValue == 10) {
			layerMode = LayerMode::Mask;
			maskCounter++;
		}
		else {
			layerMode = LayerMode::Addition;
			additionCounter++;
		}

		heightmap->addLayer(heightmapLayers.at(i), layerMode);
	}

	int applyFinalMask = halfDistribution(randomGenerator);
	if (applyFinalMask == 1) {
		heightmap->addLayer(maskGenerator.generateCircleLinearFalloffMask(mapWidth, mapHeight, multiplicationDistribution(randomGenerator) * (0.25f * mapWidth), multiplicationDistribution(randomGenerator) * (0.25f * mapWidth), multiplicationDistribution(randomGenerator) * (0.66f * mapWidth), multiplicationDistribution(randomGenerator) * (0.66f * mapHeight)), LayerMode::Mask);
	}
	if (applyFinalMask == 2) {
		heightmap->addLayer(maskGenerator.generateRectangleLinearFalloffMask(mapWidth, mapHeight, multiplicationDistribution(randomGenerator) * (0.25f * mapWidth), multiplicationDistribution(randomGenerator) * (0.25f * mapHeight), multiplicationDistribution(randomGenerator) * (0.25f * mapWidth), multiplicationDistribution(randomGenerator) * (0.66f * mapWidth), multiplicationDistribution(randomGenerator) * (0.66f * mapHeight)), LayerMode::Mask);
	}

	
	if (heightmap->getHighestElevation() - heightmap->getLowestElevation() > 220) {
		std::uniform_real_distribution<> scaleAmplitudeFactorDistribution(0.45, 0.75);
		heightmap->scaleAmplitude(scaleAmplitudeFactorDistribution(randomGenerator));
	}
	else if (heightmap->getHighestElevation() - heightmap->getLowestElevation() < 100) {
		std::uniform_real_distribution<> scaleAmplitudeFactorDistribution(1.5, 2.2);
		heightmap->scaleAmplitude(scaleAmplitudeFactorDistribution(randomGenerator));
	}

	return heightmap;
}

pcb::Heightmap pcb::LayeredHeightmapGenerator::generateHeightmap(double noiseSamplingFrequencyX, double noiseSamplingFrequencyY, double xOffset, double yOffset) const {
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

double pcb::LayeredHeightmapGenerator::generateElevationForNoiseCoordinates(double x, double y) const {
	return std::round(127.5 * (1 + noiseGenerator.getValueForCoordinates(x, y)));
}