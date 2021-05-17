#include <cmath>
#include <iostream>

#include "TerrainGenerator.hpp"

void pcb::TerrainGenerator::generate() {
	pcb::HeightMapGenerator heightMapGenerator(128, 128);
	pcb::HeightMap* heightmap = heightMapGenerator.generate();
	heightmap->print();

	delete heightmap;
}

pcb::HeightMapGenerator::HeightMapGenerator(int width, int height) : width(width), height(height), noiseGenerator() {}

pcb::HeightMap* pcb::HeightMapGenerator::generate() {

	unsigned char* noiseMap = new unsigned char[width * height];

	createInitialLayer(noiseMap, 0.025, 0.85, 0.5);
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
			noiseMap[(y * width) + x] = static_cast<unsigned char>(multiplier * round(maxValueFactor * generateValueForCoordinates(noiseInputX, noiseInputY)));
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
			noiseMap[(y * width) + x] = fmin(noiseMap[(y * width) + x] + static_cast<unsigned char>(multiplier * round(maxValueFactor * generateValueForCoordinates(noiseInputX, noiseInputY))), 255);
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
			noiseMap[(y * width) + x] = fmax(0, noiseMap[(y * width) + x] - static_cast<unsigned char>(multiplier * round(maxValueFactor * generateValueForCoordinates(noiseInputX, noiseInputY))));
		}
	}
}

double pcb::HeightMapGenerator::generateValueForCoordinates(double x, double y) {
	return 127.5 * (1 + noiseGenerator.getValueForCoordinates(x, y));
}

pcb::HeightMap::HeightMap(int width, int height, unsigned char* elevationValues) : width(width), height(height), elevationValues(new unsigned char[width * height]) {
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			this->elevationValues[(y * width) + x] = elevationValues[(y * width) + x];
		}
	}
}

pcb::HeightMap::~HeightMap() {
	delete[] elevationValues;
}

void pcb::HeightMap::print() {
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			std::cout << static_cast<short>(elevationValues[(y * width) + x]) << " ";
		}

		std::cout << "\n";
	}
}

pcb::Image* pcb::HeightMap::to24BitImage() {
	int arraySize = 3 * width * height;
	char* sourcePixels = new char[arraySize];
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			unsigned char elevationValue = elevationValues[(y * width) + x];
			int index = (3 * y * width) + (3 * x);
			sourcePixels[index] = elevationValue;
			sourcePixels[index + 1] = elevationValue;
			sourcePixels[index + 2] = elevationValue;
		}
	}

	return new Image(sourcePixels, arraySize, width, height, pcb::PixelDataFormat::RGB);
}