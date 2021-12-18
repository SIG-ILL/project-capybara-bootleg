#include "LayeredHeightmapGenerator.hpp"

#include <cmath>
#include <random>

#include "MaskGenerator.hpp"
#include "RandomHeightmapGenerator.hpp"
#include "RandomHeightmapGenerationDataGenerator.hpp"

#include "ProceduralHeightmap.hpp"
#include "ProceduralHeightmapOperations.hpp"

#include "Logger.hpp"
#include "Stopwatch.hpp"

pcb::LayeredHeightmapGenerator::LayeredHeightmapGenerator(int mapWidth, int mapHeight) : mapWidth(mapWidth), mapHeight(mapHeight), noiseGenerator() {}

std::unique_ptr<pcb::LayeredHeightmap> pcb::LayeredHeightmapGenerator::generate() const {
	// Layer 1
	std::unique_ptr<ProceduralHeightmapOperationNoise> heightmap1 = std::make_unique<ProceduralHeightmapOperationNoise>(mapWidth, mapHeight, 128, 128, 0, 0);
	std::unique_ptr<ProceduralHeightmapOperationLowerToLevel> heightmap1_2 = std::make_unique<ProceduralHeightmapOperationLowerToLevel>(std::move(heightmap1), 175);
	std::unique_ptr<ProceduralHeightmapOperationScale> layer1 = std::make_unique<ProceduralHeightmapOperationScale>(std::move(heightmap1_2), 0.5);

	// Layer 2
	std::unique_ptr<ProceduralHeightmapOperationNoise> heightmap2 = std::make_unique<ProceduralHeightmapOperationNoise>(mapWidth, mapHeight, 1024, 128, 8, 0);
	std::unique_ptr<ProceduralHeightmapOperationScale>heightmap2_1 = std::make_unique<ProceduralHeightmapOperationScale>(std::move(heightmap2), 0.5);
	std::unique_ptr<ProceduralHeightmapOperationScaleAmplitude> heightmap2_2 = std::make_unique<ProceduralHeightmapOperationScaleAmplitude>(std::move(heightmap2_1), 2);
	// Layer 2 mask
	std::unique_ptr<ProceduralHeightmapOperationCircle> maskLayer2Uncached = std::make_unique<ProceduralHeightmapOperationCircle>(mapWidth, mapHeight, (mapWidth / 256.0f) * 30, (mapWidth / 256.0f) * 98, 0, 0);
	std::shared_ptr<ProceduralHeightmapOperationModifierCacheResult> maskLayer2 = std::make_shared<ProceduralHeightmapOperationModifierCacheResult>(*maskLayer2Uncached);
	std::unique_ptr<ProceduralHeightmapOperationMask> layer2 = std::make_unique<ProceduralHeightmapOperationMask>(std::move(heightmap2_2), maskLayer2);

	std::unique_ptr<ProceduralHeightmapOperationAdd> combinedLayers1 = std::make_unique<ProceduralHeightmapOperationAdd>(std::move(layer1), std::move(layer2));

	// Layer 3
	std::unique_ptr<ProceduralHeightmapOperationNoise> heightmap3 = std::make_unique<ProceduralHeightmapOperationNoise>(mapWidth, mapHeight, 32, 32, 0, 8);
	std::unique_ptr<ProceduralHeightmapOperationScale> layer3Uncached = std::make_unique<ProceduralHeightmapOperationScale>(std::move(heightmap3), 0.0625);
	std::shared_ptr<ProceduralHeightmapOperationModifierCacheResult> layer3 = std::make_shared<ProceduralHeightmapOperationModifierCacheResult>(*layer3Uncached);

	std::unique_ptr<ProceduralHeightmapOperationAdd> combinedLayers2 = std::make_unique<ProceduralHeightmapOperationAdd>(std::move(combinedLayers1), layer3);

	// Layer 4
	std::unique_ptr<ProceduralHeightmapOperationNoise> heightmap4 = std::make_unique<ProceduralHeightmapOperationNoise>(mapWidth, mapHeight, 16, 16, 8, 8);
	std::unique_ptr<ProceduralHeightmapOperationScale> layer4 = std::make_unique<ProceduralHeightmapOperationScale>(std::move(heightmap4), 0.03125);

	std::unique_ptr<ProceduralHeightmapOperationAdd> combinedLayers3 = std::make_unique<ProceduralHeightmapOperationAdd>(std::move(combinedLayers2), std::move(layer4));

	// Global heightmap scale
	std::unique_ptr<ProceduralHeightmapOperationScale> combinedLayers3_1 = std::make_unique<ProceduralHeightmapOperationScale>(std::move(combinedLayers3), 1.5);

	// Layer 5
	std::unique_ptr<ProceduralHeightmapOperationMask> combinedLayers4 = std::make_unique<ProceduralHeightmapOperationMask>(std::move(combinedLayers3_1), maskLayer2);

	// Layer 6
	std::unique_ptr<ProceduralHeightmapOperationRectangle> heightmap6 = std::make_unique<ProceduralHeightmapOperationRectangle>(mapWidth, mapHeight, (mapWidth / 256.0f) * 15, (mapHeight / 256.0f) * 15, (mapWidth / 256.0f) * 64, 0, 0);
	std::unique_ptr<ProceduralHeightmapOperationInvert> layer6 = std::make_unique<ProceduralHeightmapOperationInvert>(std::move(heightmap6));

	std::unique_ptr<ProceduralHeightmapOperationMask> combinedLayers5Uncached = std::make_unique<ProceduralHeightmapOperationMask>(std::move(combinedLayers4), std::move(layer6));
	std::shared_ptr<ProceduralHeightmapOperationModifierCacheResult> combinedLayers5 = std::make_shared<ProceduralHeightmapOperationModifierCacheResult>(*combinedLayers5Uncached);

	// Layer 7
	std::unique_ptr<ProceduralHeightmapOperationNoise> heightmap7 = std::make_unique<ProceduralHeightmapOperationNoise>(mapWidth, mapHeight, 16, 16, 0, 0);
	std::unique_ptr<ProceduralHeightmapOperationMask> heightmap7_1 = std::make_unique<ProceduralHeightmapOperationMask>(std::move(heightmap7), combinedLayers5);
	// Layer 7 mask
	std::unique_ptr<ProceduralHeightmapOperationInvert> maskLayer7 = std::make_unique<ProceduralHeightmapOperationInvert>(maskLayer2);
	std::unique_ptr<ProceduralHeightmapOperationMask> layer7 = std::make_unique<ProceduralHeightmapOperationMask>(std::move(heightmap7_1), std::move(maskLayer7));

	std::unique_ptr<ProceduralHeightmapOperationAdd> combinedLayers6 = std::make_unique<ProceduralHeightmapOperationAdd>(combinedLayers5, std::move(layer7));

	// Layer 8
	std::unique_ptr<ProceduralHeightmapOperationScale> layer8 = std::make_unique<ProceduralHeightmapOperationScale>(layer3, 10);

	std::unique_ptr<ProceduralHeightmapOperationMask> finalOperation = std::make_unique<ProceduralHeightmapOperationMask>(std::move(combinedLayers6), std::move(layer8));

	ProceduralHeightmap proceduralHeightmap(std::move(finalOperation));

	std::unique_ptr<Heightmap> result = proceduralHeightmap.generateResult();
	std::unique_ptr<LayeredHeightmap> returnValue = std::make_unique<LayeredHeightmap>(mapWidth, mapHeight);
	returnValue->addLayer(std::move(result), LayerMode::Addition);

	return returnValue;
}

std::unique_ptr<pcb::LayeredHeightmap> pcb::LayeredHeightmapGenerator::generateRandom() const {
	RandomHeightmapGenerator generator(mapWidth, mapHeight);
	RandomHeightmapGenerationDataGenerator dataGenerator;
	RandomHeightmapGenerationData generationData = dataGenerator.generate(mapWidth, mapHeight);

	//return generator.generate(generationData);

	Stopwatch stopwatch;
	stopwatch.start();
	std::unique_ptr<pcb::LayeredHeightmap> returnValue = generator.generate(generationData);
	stopwatch.stop();
	Logger logger;
	logger << "Layered Random Heightmap generated in " << static_cast<int>(stopwatch.getLastClockedDurationInMilliseconds()) << "ms.\n";
	logger << "This is an average of " << static_cast<int>(stopwatch.getLastClockedDurationInMilliseconds() / generationData.layerData->size()) << " ms per layer (" << generationData.layerData->size() << " layers).\n";

	return returnValue;
}

std::unique_ptr<pcb::Heightmap> pcb::LayeredHeightmapGenerator::generateHeightmap(double noiseSamplingFrequencyX, double noiseSamplingFrequencyY, double xOffset, double yOffset) const {
	std::shared_ptr<std::vector<unsigned char>> noiseMap = std::make_shared<std::vector<unsigned char>>();
	noiseMap->reserve(mapWidth * mapHeight);
	const double samplingFrequencyToSamplingDistanceModifierX = 1.0 / noiseSamplingFrequencyX;
	const double samplingFrequencyToSamplingDistanceModifierY = 1.0 / noiseSamplingFrequencyY;

	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			double noiseInputX = xOffset + (x * samplingFrequencyToSamplingDistanceModifierX);
			double noiseInputY = yOffset + (y * samplingFrequencyToSamplingDistanceModifierY);
			noiseMap->push_back(static_cast<unsigned char>(generateElevationForNoiseCoordinates(noiseInputX, noiseInputY)));
		}
	}

	return std::make_unique<Heightmap>(mapWidth, mapHeight, noiseMap);
}

double pcb::LayeredHeightmapGenerator::generateElevationForNoiseCoordinates(double x, double y) const {
	return std::round(127.5 * (1 + noiseGenerator.getValueForCoordinate(x, y)));
}