#include "Heightmap.hpp"

#include <algorithm>

pcb::Heightmap::Heightmap(int width, int height, unsigned char* elevationValues) : width(width), height(height), elevationValues(new unsigned char[width * height]) {
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			this->elevationValues[(y * width) + x] = elevationValues[(y * width) + x];
		}
	}
}

pcb::Heightmap::Heightmap(const pcb::Heightmap& other) : width(other.width), height(other.height), elevationValues(new unsigned char[width * height]) {
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			elevationValues[(y * width) + x] = other.elevationValues[(y * width) + x];
		}
	}
}

pcb::Heightmap::~Heightmap() {
	delete[] elevationValues;
}

pcb::Heightmap& pcb::Heightmap::operator+=(const pcb::Heightmap& other) {
	add(other);
	return *this;
}

pcb::Heightmap& pcb::Heightmap::operator-=(const pcb::Heightmap& other) {
	subtract(other);
	return *this;
}

pcb::Image* pcb::Heightmap::to24BitImageNew() const {
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

unsigned char pcb::Heightmap::getValueAt(int x, int y) const {
	return elevationValues[(y * width) + x];
}

int pcb::Heightmap::getWidth() const {
	return width;
}

int pcb::Heightmap::getHeight() const {
	return height;
}

void pcb::Heightmap::add(const pcb::Heightmap& other) {
	int heightConditionValue = static_cast<int>(std::min(height, other.height));
	int widthConditionValue = static_cast<int>(std::min(width, other.width));

	for (int y = 0; y < heightConditionValue; y++) {
		for (int x = 0; x < widthConditionValue; x++) {
			this->elevationValues[(y * width) + x] = static_cast<unsigned char>(std::min(this->elevationValues[(y * width) + x] + other.elevationValues[(y * other.width) + x], 255));
		}
	}
}

void pcb::Heightmap::subtract(const pcb::Heightmap& other) {
	int heightConditionValue = static_cast<int>(std::min(height, other.height));
	int widthConditionValue = static_cast<int>(std::min(width, other.width));

	for (int y = 0; y < heightConditionValue; y++) {
		for (int x = 0; x < widthConditionValue; x++) {
			this->elevationValues[(y * width) + x] = static_cast<unsigned char>(std::max(0, this->elevationValues[(y * width) + x] - other.elevationValues[(y * other.width) + x]));
		}
	}
}