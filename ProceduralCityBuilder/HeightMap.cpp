#include "Heightmap.hpp"

#include <algorithm>
#include <vector>
#include <stdexcept>
#include <cmath>

pcb::Heightmap::Heightmap(const int width, const int height) : width(width), height(height), lowestElevation(MIN_ELEVATION_VALUE), highestElevation(MIN_ELEVATION_VALUE), elevationValues(std::make_unique<std::vector<unsigned char>>(width * height, MIN_ELEVATION_VALUE)) {}

pcb::Heightmap::Heightmap(const int width, const int height, std::shared_ptr<std::vector<unsigned char>> elevationValues) : width(width), height(height), lowestElevation(MAX_ELEVATION_VALUE), highestElevation(MIN_ELEVATION_VALUE), elevationValues(elevationValues) {
	std::pair minMaxElevation = std::minmax_element(elevationValues->begin(), elevationValues->end());
	lowestElevation = *(minMaxElevation.first);
	highestElevation = *(minMaxElevation.second);
}

pcb::Heightmap::Heightmap(const pcb::Image& image) : width(image.getWidth()), height(image.getHeight()), lowestElevation(MIN_ELEVATION_VALUE), highestElevation(MAX_ELEVATION_VALUE), elevationValues(std::make_unique<std::vector<unsigned char>>(image.getWidth() * image.getHeight(), MIN_ELEVATION_VALUE)) {
	std::shared_ptr<std::vector<char>> imagePixels = image.getPixels();
	const int BYTES_PER_PIXEL = 3;
	for (int i = 0; i < elevationValues->size(); i++) {
		(*elevationValues)[i] = (*imagePixels)[i * BYTES_PER_PIXEL];
	}

	std::pair minMaxElevation = std::minmax_element(elevationValues->begin(), elevationValues->end());
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

std::unique_ptr<pcb::Image> pcb::Heightmap::to24BitImage() const {
	const int COLOR_CHANNELS = 3;
	int vectorSize = COLOR_CHANNELS * width * height;
	std::unique_ptr<std::vector<char>> sourcePixels = std::make_unique<std::vector<char>>(vectorSize, 0);

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int index = (y * COLOR_CHANNELS * width) + (COLOR_CHANNELS * x);
			unsigned char elevationValue = elevationValues->at((y * width) + x);
			(*sourcePixels)[index] = elevationValue;
			(*sourcePixels)[index + 1] = elevationValue;
			(*sourcePixels)[index + 2] = elevationValue;
		}
	}

	return std::make_unique<Image>(std::move(sourcePixels), width, height, pcb::PixelDataFormat::RGB);
}

unsigned char pcb::Heightmap::getValueAt(int x, int y) const {
	return elevationValues->at((y * width) + x);
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
		int indexRowStartIndex = (y * width);

		for (int x = 0; x < widthConditionValue; x++) {
			int index = indexRowStartIndex + x;
			unsigned char newValue = static_cast<unsigned char>(std::min(this->elevationValues->at(index) + other.elevationValues->at((y * other.width) + x), 255));
			this->elevationValues->at(index) = newValue;

			if (newValue > highestElevation) {
				highestElevation = newValue;
			}
		}
	}

	lowestElevation = *std::min_element(this->elevationValues->begin(), this->elevationValues->end());
}

void pcb::Heightmap::subtract(const pcb::Heightmap& other) {
	const int heightConditionValue = std::min(height, other.height);
	const int widthConditionValue = std::min(width, other.width);

	for (int y = 0; y < heightConditionValue; y++) {
		int indexRowStartIndex = (y * width);

		for (int x = 0; x < widthConditionValue; x++) {
			int index = indexRowStartIndex + x;
			unsigned char newValue = static_cast<unsigned char>(std::max(0, this->elevationValues->at(index) - other.elevationValues->at((y * other.width) + x)));
			this->elevationValues->at(index) = newValue;

			if (newValue < lowestElevation) {
				lowestElevation = newValue;
			}
		}
	}

	highestElevation = *std::max_element(this->elevationValues->begin(), this->elevationValues->end());
}

void pcb::Heightmap::raiseToLevel(const unsigned char level) {
	if (level < MIN_ELEVATION_VALUE || level > MAX_ELEVATION_VALUE) {
		throw std::invalid_argument("Level argument out of allowed elevation range.");
	}

	for (int i = 0; i < (width * height); i++) {
		elevationValues->at(i) = std::max(elevationValues->at(i), level);
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
		elevationValues->at(i) = std::min(elevationValues->at(i), level);
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
		int indexRowStartIndex = (y * width);

		for (int x = 0; x < widthConditionValue; x++) {
			int index = indexRowStartIndex + x;
			this->elevationValues->at(index) = std::clamp(static_cast<unsigned char>(std::round(mask.elevationValues->at((y * mask.width) + x) * (maskNormalizationValue * this->elevationValues->at(index)))), MIN_ELEVATION_VALUE, MAX_ELEVATION_VALUE);
		}
	}

	std::pair minMaxElevation = std::minmax_element(this->elevationValues->begin(), this->elevationValues->end());
	lowestElevation = *(minMaxElevation.first);
	highestElevation = *(minMaxElevation.second);
}

void pcb::Heightmap::scale(const double factor) {
	if (factor < 0.0) {
		throw std::invalid_argument("Factor argument is negative. Only positive values are allowed.");
	}

	for (int i = 0; i < (width * height); i++) {
		elevationValues->at(i) = static_cast<unsigned char>(std::min(std::round(factor * elevationValues->at(i)), static_cast<double>(MAX_ELEVATION_VALUE)));
	}

	std::pair minMaxElevation = std::minmax_element(this->elevationValues->begin(), this->elevationValues->end());
	lowestElevation = *(minMaxElevation.first);
	highestElevation = *(minMaxElevation.second);
}

void pcb::Heightmap::invert() {
	for (int i = 0; i < (width * height); i++) {
		elevationValues->at(i) = static_cast<unsigned char>(-1 * (elevationValues->at(i) - MAX_ELEVATION_VALUE));
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
		elevationValues->at(i) = static_cast<unsigned char>(std::clamp(std::round((factor * (elevationValues->at(i) - scalingAxisValue)) + scalingAxisValue), static_cast<double>(MIN_ELEVATION_VALUE), static_cast<double>(MAX_ELEVATION_VALUE)));
	}

	std::pair minMaxElevation = std::minmax_element(this->elevationValues->begin(), this->elevationValues->end());
	lowestElevation = *(minMaxElevation.first);
	highestElevation = *(minMaxElevation.second);
}

void pcb::Heightmap::raise(const unsigned char amount) {
	if (amount < 0) {
		throw std::invalid_argument("Amount is not allowed to be negative!");
	}

	for (int i = 0; i < (width * height); i++) {
		elevationValues->at(i) = static_cast<unsigned char>(std::min(elevationValues->at(i) + amount, static_cast<int>(MAX_ELEVATION_VALUE)));
	}

	lowestElevation = static_cast<unsigned char>(std::min(lowestElevation + amount, static_cast<int>(MAX_ELEVATION_VALUE)));
	highestElevation = static_cast<unsigned char>(std::min(highestElevation + amount, static_cast<int>(MAX_ELEVATION_VALUE)));
}

void pcb::Heightmap::lower(const unsigned char amount) {
	if (amount < 0) {
		throw std::invalid_argument("Amount is not allowed to be negative!");
	}

	for (int i = 0; i < (width * height); i++) {
		elevationValues->at(i) = static_cast<unsigned char>(std::max(elevationValues->at(i) - amount, static_cast<int>(MIN_ELEVATION_VALUE)));
	}

	lowestElevation = static_cast<unsigned char>(std::max(lowestElevation - amount, static_cast<int>(MIN_ELEVATION_VALUE)));
	highestElevation = static_cast<unsigned char>(std::max(highestElevation - amount, static_cast<int>(MIN_ELEVATION_VALUE)));
}

void pcb::Heightmap::toBlackAndWhite(const unsigned char inclusiveThreshold) {
	if (inclusiveThreshold < MIN_ELEVATION_VALUE || inclusiveThreshold > MAX_ELEVATION_VALUE) {
		throw std::invalid_argument("Threshold is not allowed to be outside [MIN_ELEVATION_VALUE; MAX_ELEVATION_VALUE] range!");
	}

	for (int i = 0; i < (width * height); i++) {
		elevationValues->at(i) = elevationValues->at(i) >= inclusiveThreshold ? MAX_ELEVATION_VALUE : MIN_ELEVATION_VALUE;
	}

	std::pair minMaxElevation = std::minmax_element(this->elevationValues->begin(), this->elevationValues->end());
	lowestElevation = *(minMaxElevation.first);
	highestElevation = *(minMaxElevation.second);
}

void pcb::Heightmap::flatten(const unsigned char amountOfBuckets) {
	if (amountOfBuckets < 2) {
		throw std::invalid_argument("amountOfBuckets is not allowed to be less than 2!");
	}

	int bucketSize = (MAX_ELEVATION_VALUE + 1) / amountOfBuckets;
	int valueStepSize = MAX_ELEVATION_VALUE / (amountOfBuckets - 1);

	for (int i = 0; i < (width * height); i++) {
		elevationValues->at(i) = static_cast<unsigned char>(std::floor(std::floor(elevationValues->at(i) / bucketSize) * valueStepSize));
	}

	std::pair minMaxElevation = std::minmax_element(this->elevationValues->begin(), this->elevationValues->end());
	lowestElevation = *(minMaxElevation.first);
	highestElevation = *(minMaxElevation.second);
}