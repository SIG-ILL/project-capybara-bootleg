#include "HeightMap.hpp"

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

pcb::Image* pcb::HeightMap::to24BitImageNew() {
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

	Image* image = new Image(sourcePixels, arraySize, width, height, pcb::PixelDataFormat::RGB);
	delete[] sourcePixels;

	return image;
}

unsigned char pcb::HeightMap::getValueAt(int x, int y) {
	return elevationValues[(y * width) + x];
}

int pcb::HeightMap::getWidth() {
	return width;
}

int pcb::HeightMap::getHeight() {
	return height;
}