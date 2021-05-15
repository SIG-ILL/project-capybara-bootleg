#include <cmath>
#include <iostream>

#include "TerrainGenerator.hpp"
#include "SimplexNoise.hpp"

void pcb::TerrainGenerator::generate() {
	pcb::HeightMapGenerator heightMapGenerator;
	pcb::HeightMap* heightmap = heightMapGenerator.generate();
	heightmap->print();

	delete heightmap;
}

pcb::HeightMap* pcb::HeightMapGenerator::generate() {
	SimplexNoise noise;

	unsigned char* noiseMap = new unsigned char[128 * 128];

	for (int y = 0; y < 128; y++) {
		for (int x = 0; x < 128; x++) {
			double noiseModifier = 0.025;
			double noiseInputX = x * noiseModifier;
			double noiseInputY = y * noiseModifier;
			noiseMap[(y * 128) + x] = static_cast<unsigned char>(round(127.5 * (1 + noise.getValueForCoordinates(noiseInputX, noiseInputY))));
		}		
	}

	for (int y = 0; y < 128; y++) {
		for (int x = 0; x < 128; x++) {
			double noiseModifier = 0.05;
			double noiseInputX = x * noiseModifier;
			double noiseInputY = y * noiseModifier;
			noiseMap[(y * 128) + x] |= static_cast<unsigned char>(0.5 * round(127.5 * (1 + noise.getValueForCoordinates(noiseInputX, noiseInputY))));
		}
	}

	for (int y = 0; y < 128; y++) {
		for (int x = 0; x < 128; x++) {
			double noiseModifier = 0.1;
			double noiseInputX = x * noiseModifier;
			double noiseInputY = y * noiseModifier;
			noiseMap[(y * 128) + x] |= static_cast<unsigned char>(0.25 * round(127.5 * (1 + noise.getValueForCoordinates(noiseInputX, noiseInputY))));
		}
	}

	HeightMap* heightMap = new HeightMap(128, 128, noiseMap);
	delete[] noiseMap;

	return heightMap;
}

pcb::HeightMap::HeightMap(unsigned int width, unsigned int height, unsigned char* elevationValues) : width(width), height(height), elevationValues(new unsigned char[width * height]) {
	for (unsigned int y = 0; y < height; y++) {
		for (unsigned int x = 0; x < width; x++) {
			this->elevationValues[(y * width) + x] = elevationValues[(y * width) + x];
		}
	}
}

pcb::HeightMap::~HeightMap() {
	delete[] elevationValues;
}

void pcb::HeightMap::print() {
	for (unsigned int y = 0; y < height; y++) {
		for (unsigned int x = 0; x < width; x++) {
			std::cout << static_cast<short>(elevationValues[(y * width) + x]) << " ";
		}

		std::cout << "\n";
	}
}

pcb::Image* pcb::HeightMap::to24BitImage() {
	int arraySize = 3 * width * height;
	char* sourcePixels = new char[arraySize];
	for (unsigned int y = 0; y < height; y++) {
		for (unsigned int x = 0; x < width; x++) {
			unsigned char elevationValue = elevationValues[(y * width) + x];
			int index = (3 * y * width) + (3 * x);
			sourcePixels[index] = elevationValue;
			sourcePixels[index + 1] = elevationValue;
			sourcePixels[index + 2] = elevationValue;
		}
	}

	return new Image(sourcePixels, arraySize, 128, 128, pcb::PixelDataFormat::RGB);
}