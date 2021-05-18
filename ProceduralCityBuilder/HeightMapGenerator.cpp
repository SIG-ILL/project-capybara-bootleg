#include "HeightMapGenerator.hpp"

#include <cmath>

pcb::HeightMapGenerator::HeightMapGenerator(int width, int height) : width(width), height(height), noiseGenerator() {}

pcb::HeightMap* pcb::HeightMapGenerator::generateNew() {

	unsigned char* noiseMap = new unsigned char[width * height];

	//createInitialLayer(noiseMap, 0.025, 0.85, 0.5);
	createInitialLayer(noiseMap, 0.025, 0.85, 1);
	addAdditiveLayer(noiseMap, 0.05, 0.25, 0.4);
	addAdditiveLayer(noiseMap, 0.1, 0.1, 0.3);
	addAdditiveLayer(noiseMap, 0.25, 0.15, 0.2);

	addSubtractiveLayer(noiseMap, 0.05, 0.15, 0.2);
	addSubtractiveLayer(noiseMap, 0.1, 0.3, 0.1);
	addSubtractiveLayer(noiseMap, 0.25, 0.55, 0.4);

	HeightMap* heightMap = new HeightMap(width, height, noiseMap);
	delete[] noiseMap;

	return heightMap;
}

void pcb::HeightMapGenerator::createInitialLayer(unsigned char* noiseMap, double noiseModifier, double multiplier, double maxValueFactor) {
	if (maxValueFactor > 1.0) {
		maxValueFactor = 1.0;
	}
	if (maxValueFactor < 0.0) {
		maxValueFactor = 0.0;
	}

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			double noiseInputX = x * noiseModifier;
			double noiseInputY = y * noiseModifier;
			noiseMap[(y * width) + x] = static_cast<unsigned char>(multiplier * std::round(maxValueFactor * generateValueForCoordinates(noiseInputX, noiseInputY)));
		}
	}
}

void pcb::HeightMapGenerator::addAdditiveLayer(unsigned char* noiseMap, double noiseModifier, double multiplier, double maxValueFactor) {
	if (maxValueFactor > 1.0) {
		maxValueFactor = 1.0;
	}
	if (maxValueFactor < 0.0) {
		maxValueFactor = 0.0;
	}

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			double noiseInputX = x * noiseModifier;
			double noiseInputY = y * noiseModifier;
			noiseMap[(y * width) + x] = static_cast<unsigned char>(std::fmin(noiseMap[(y * width) + x] + static_cast<unsigned char>(multiplier * std::round(maxValueFactor * generateValueForCoordinates(noiseInputX, noiseInputY))), 255));
		}
	}
}

void pcb::HeightMapGenerator::addSubtractiveLayer(unsigned char* noiseMap, double noiseModifier, double multiplier, double maxValueFactor) {
	if (maxValueFactor > 1.0) {
		maxValueFactor = 1.0;
	}
	if (maxValueFactor < 0.0) {
		maxValueFactor = 0.0;
	}

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			double noiseInputX = x * noiseModifier;
			double noiseInputY = y * noiseModifier;
			noiseMap[(y * width) + x] = static_cast<unsigned char>(std::fmax(0, noiseMap[(y * width) + x] - static_cast<unsigned char>(multiplier * std::round(maxValueFactor * generateValueForCoordinates(noiseInputX, noiseInputY)))));
		}
	}
}

double pcb::HeightMapGenerator::generateValueForCoordinates(double x, double y) {
	return 127.5 * (1 + noiseGenerator.getValueForCoordinates(x, y));
}