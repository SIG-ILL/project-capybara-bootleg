#include "Heightmap.hpp"

#include <algorithm>
#include <vector>
#include <stdexcept>
#include <cmath>

pcb::Heightmap::Heightmap(const int width, const int height) : width(width), height(height), lowestElevation(MIN_ELEVATION_VALUE), highestElevation(MIN_ELEVATION_VALUE), elevationValues(width * height, MIN_ELEVATION_VALUE) {}

pcb::Heightmap::Heightmap(const int width, const int height, const std::vector<unsigned char>& elevationValues) : width(width), height(height), lowestElevation(MAX_ELEVATION_VALUE), highestElevation(MIN_ELEVATION_VALUE), elevationValues(elevationValues) {
	std::pair minMaxElevation = std::minmax_element(elevationValues.begin(), elevationValues.end());
	lowestElevation = *(minMaxElevation.first);
	highestElevation = *(minMaxElevation.second);
}

pcb::Heightmap& pcb::Heightmap::operator+=(const pcb::Heightmap& other) {
	add(other);
	return *this;
}

pcb::Heightmap& pcb::Heightmap::operator-=(const pcb::Heightmap& other) {
	subtract(other);
	return *this;
}

pcb::Image pcb::Heightmap::to24BitImage() const {
	int vectorSize = 3 * width * height;
	std::vector<char> sourcePixels;
	sourcePixels.reserve(vectorSize);

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			unsigned char elevationValue = elevationValues.at((y * width) + x);
			sourcePixels.push_back(elevationValue);
			sourcePixels.push_back(elevationValue);
			sourcePixels.push_back(elevationValue);
		}
	}

	Image image(sourcePixels, width, height, pcb::PixelDataFormat::RGB);

	return image;
}

unsigned char pcb::Heightmap::getValueAt(int x, int y) const {
	return elevationValues.at((y * width) + x);
}

int pcb::Heightmap::getWidth() const {
	return width;
}

int pcb::Heightmap::getHeight() const {
	return height;
}

int pcb::Heightmap::getLowestElevation() const {
	return lowestElevation;
}

int pcb::Heightmap::getHighestElevation() const {
	return highestElevation;
}

void pcb::Heightmap::add(const pcb::Heightmap& other) {
	const int heightConditionValue = std::min(height, other.height);
	const int widthConditionValue = std::min(width, other.width);

	for (int y = 0; y < heightConditionValue; y++) {
		for (int x = 0; x < widthConditionValue; x++) {
			this->elevationValues.at((y * width) + x) = static_cast<unsigned char>(std::min(this->elevationValues.at((y * width) + x) + other.elevationValues.at((y * other.width) + x), 255));

			if (this->elevationValues.at((y * width) + x) > highestElevation) {
				highestElevation = this->elevationValues.at((y * width) + x);
			}
		}
	}

	lowestElevation = *std::min_element(this->elevationValues.begin(), this->elevationValues.end());
}

void pcb::Heightmap::subtract(const pcb::Heightmap& other) {
	const int heightConditionValue = std::min(height, other.height);
	const int widthConditionValue = std::min(width, other.width);

	for (int y = 0; y < heightConditionValue; y++) {
		for (int x = 0; x < widthConditionValue; x++) {
			this->elevationValues.at((y * width) + x) = static_cast<unsigned char>(std::max(0, this->elevationValues.at((y * width) + x) - other.elevationValues.at((y * other.width) + x)));

			if (this->elevationValues[(y * width) + x] < lowestElevation) {
				lowestElevation = this->elevationValues[(y * width) + x];
			}
		}
	}

	highestElevation = *std::max_element(this->elevationValues.begin(), this->elevationValues.end());
}

void pcb::Heightmap::raiseToLevel(const unsigned char level) {
	if (level < MIN_ELEVATION_VALUE || level > MAX_ELEVATION_VALUE) {
		throw std::invalid_argument("Level argument out of allowed elevation range.");
	}

	for (int i = 0; i < (width * height); i++) {
		elevationValues.at(i) = std::max(elevationValues.at(i), level);
	}

	if (lowestElevation < level) {
		lowestElevation = level;
	}
}

void pcb::Heightmap::lowerToLevel(const unsigned char level) {
	if (level < MIN_ELEVATION_VALUE || level > MAX_ELEVATION_VALUE) {
		throw std::invalid_argument("Level argument out of allowed elevation range.");
	}

	for (int i = 0; i < (width * height); i++) {
		elevationValues.at(i) = std::min(elevationValues.at(i), level);
	}

	if (highestElevation > level) {
		highestElevation = level;
	}
}

void pcb::Heightmap::clamp(const unsigned char lowerLevel, const unsigned char upperLevel) {
	if (lowerLevel < MIN_ELEVATION_VALUE || upperLevel > MAX_ELEVATION_VALUE || lowerLevel >= upperLevel) {
		throw std::invalid_argument("Level argument out of allowed elevation range, or lower level is equal to or higher than upper level.");
	}

	raiseToLevel(lowerLevel);
	lowerToLevel(upperLevel);
}

void pcb::Heightmap::mask(const pcb::Heightmap& mask) {
	const int heightConditionValue = std::min(height, mask.height);
	const int widthConditionValue = std::min(width, mask.width);
	const double maskNormalizationValue = 1.0 / 255;

	for (int y = 0; y < heightConditionValue; y++) {
		for (int x = 0; x < widthConditionValue; x++) {
			this->elevationValues.at((y * width) + x) = std::clamp(static_cast<unsigned char>(std::round(mask.elevationValues.at((y * mask.width) + x) * (maskNormalizationValue * this->elevationValues.at((y * width) + x)))), MIN_ELEVATION_VALUE, MAX_ELEVATION_VALUE);
		}
	}

	std::pair minMaxElevation = std::minmax_element(this->elevationValues.begin(), this->elevationValues.end());
	lowestElevation = *(minMaxElevation.first);
	highestElevation = *(minMaxElevation.second);
}

void pcb::Heightmap::scale(const double factor) {
	if (factor < 0.0) {
		throw std::invalid_argument("Factor argument is negative. Only positive values are allowed.");
	}

	for (int i = 0; i < (width * height); i++) {
		elevationValues.at(i) = static_cast<unsigned char>(std::min(std::round(factor * elevationValues.at(i)), static_cast<double>(MAX_ELEVATION_VALUE)));
	}

	std::pair minMaxElevation = std::minmax_element(this->elevationValues.begin(), this->elevationValues.end());
	lowestElevation = *(minMaxElevation.first);
	highestElevation = *(minMaxElevation.second);
}

void pcb::Heightmap::invert() {
	for (int i = 0; i < (width * height); i++) {
		elevationValues.at(i) = static_cast<unsigned char>(-1 * (elevationValues.at(i) - MAX_ELEVATION_VALUE));
	}

	lowestElevation = static_cast<unsigned char>(-1 * (highestElevation - MAX_ELEVATION_VALUE));
	highestElevation = static_cast<unsigned char>(-1 * (lowestElevation - MAX_ELEVATION_VALUE));
}

void pcb::Heightmap::scaleAmplitude(const double factor) {
	if (factor < 0.0) {
		throw std::invalid_argument("Factor argument is negative. Only positive values are allowed.");
	}

	double scalingAxisValue = lowestElevation + (0.5 * (highestElevation - lowestElevation));
	for (int i = 0; i < (width * height); i++) {
		elevationValues.at(i) = static_cast<unsigned char>(std::clamp(std::round((factor * (elevationValues.at(i) - scalingAxisValue)) + scalingAxisValue), static_cast<double>(MIN_ELEVATION_VALUE), static_cast<double>(MAX_ELEVATION_VALUE)));
	}

	std::pair minMaxElevation = std::minmax_element(this->elevationValues.begin(), this->elevationValues.end());
	lowestElevation = *(minMaxElevation.first);
	highestElevation = *(minMaxElevation.second);
}

void pcb::Heightmap::raise(const unsigned char amount) {
	if (amount < 0) {
		throw std::invalid_argument("Amount is not allowed to be negative!");
	}

	for (int i = 0; i < (width * height); i++) {
		elevationValues.at(i) = static_cast<unsigned char>(std::min(elevationValues.at(i) + amount, static_cast<int>(MAX_ELEVATION_VALUE)));
	}

	lowestElevation = static_cast<unsigned char>(std::min(lowestElevation + amount, static_cast<int>(MAX_ELEVATION_VALUE)));
	highestElevation = static_cast<unsigned char>(std::min(highestElevation + amount, static_cast<int>(MAX_ELEVATION_VALUE)));
}

void pcb::Heightmap::lower(const unsigned char amount) {
	if (amount < 0) {
		throw std::invalid_argument("Amount is not allowed to be negative!");
	}

	for (int i = 0; i < (width * height); i++) {
		elevationValues.at(i) = static_cast<unsigned char>(std::max(elevationValues.at(i) - amount, static_cast<int>(MIN_ELEVATION_VALUE)));
	}

	lowestElevation = static_cast<unsigned char>(std::max(lowestElevation - amount, static_cast<int>(MIN_ELEVATION_VALUE)));
	highestElevation = static_cast<unsigned char>(std::max(highestElevation - amount, static_cast<int>(MIN_ELEVATION_VALUE)));
}