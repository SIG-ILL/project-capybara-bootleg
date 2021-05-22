#include "Heightmap.hpp"

#include <algorithm>
#include <vector>

pcb::Heightmap::Heightmap(const int width, const int height, const unsigned char* elevationValues) : width(width), height(height), highestElevation(0), elevationValues(new unsigned char[width * height]) {
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int index = (y * width) + x;
			this->elevationValues[index] = elevationValues[index];

			if (elevationValues[index] > highestElevation) {
				highestElevation = elevationValues[index];
			}
		}
	}
}

pcb::Heightmap::Heightmap(const pcb::Heightmap& other) : Heightmap(other.width, other.height, other.elevationValues) {}

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
	std::vector<char> sourcePixels;
	sourcePixels.reserve(arraySize);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			unsigned char elevationValue = elevationValues[(y * width) + x];
			sourcePixels.push_back(elevationValue);
			sourcePixels.push_back(elevationValue);
			sourcePixels.push_back(elevationValue);
		}
	}

	Image* image = new Image(sourcePixels.data(), arraySize, width, height, pcb::PixelDataFormat::RGB);

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

int pcb::Heightmap::getHighestElevation() const {
	return highestElevation;
}

void pcb::Heightmap::add(const pcb::Heightmap& other) {
	int heightConditionValue = std::min(height, other.height);
	int widthConditionValue = std::min(width, other.width);

	for (int y = 0; y < heightConditionValue; y++) {
		for (int x = 0; x < widthConditionValue; x++) {
			this->elevationValues[(y * width) + x] = static_cast<unsigned char>(std::min(this->elevationValues[(y * width) + x] + other.elevationValues[(y * other.width) + x], 255));

			if (this->elevationValues[(y * width) + x] > highestElevation) {
				highestElevation = this->elevationValues[(y * width) + x];
			}
		}
	}
}

void pcb::Heightmap::subtract(const pcb::Heightmap& other) {
	int heightConditionValue = std::min(height, other.height);
	int widthConditionValue = std::min(width, other.width);
	highestElevation = 0;

	for (int y = 0; y < heightConditionValue; y++) {
		for (int x = 0; x < widthConditionValue; x++) {
			this->elevationValues[(y * width) + x] = static_cast<unsigned char>(std::max(0, this->elevationValues[(y * width) + x] - other.elevationValues[(y * other.width) + x]));
		}
	}
	highestElevation = *std::max_element(this->elevationValues, this->elevationValues + (width * height));
}