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
			double normalizedX = x / 128.0;
			double normalizedY = y / 128.0;
			noiseMap[(y * 128) + x] = static_cast<unsigned char>(round(127.5 * (1 + noise.getValueForCoordinates(normalizedX, normalizedY))));
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

/*pcb::Image* pcb::HeightMap::to24BitImage() {
	char* sourcePixels = 

	return new Image();
}*/