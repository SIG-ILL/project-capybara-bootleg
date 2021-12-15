#include "ProceduralHeightmapOperationGradient.hpp"

#include <stdexcept>
#include <cmath>

pcb::ProceduralHeightmapOperationGradient::ProceduralHeightmapOperationGradient(int width, int height, GradientDirection direction)
	: width(width), height(height), direction(direction) {}

std::unique_ptr<pcb::Heightmap> pcb::ProceduralHeightmapOperationGradient::generateResult() const {
	if (width < 0 || height < 0) {
		throw std::invalid_argument("An argument has a negative value. Only zero and positive values are allowed.");
	}

	std::unique_ptr<std::vector<unsigned char>> gradientData = std::make_unique<std::vector<unsigned char>>(width * height, 0);

	// The different directions have different functions for performance/optimization reasons.
	if (direction == GradientDirection::Down) {
		generateLinearGradientMaskDown(*gradientData);
	}
	else if (direction == GradientDirection::Up) {
		generateLinearGradientMaskUp(*gradientData);
	}
	else if (direction == GradientDirection::Left) {
		generateLinearGradientMaskLeft(*gradientData);
	}
	else {
		generateLinearGradientMaskRight(*gradientData);
	}

	return std::make_unique<Heightmap>(width, height, std::move(gradientData));
}

void pcb::ProceduralHeightmapOperationGradient::generateLinearGradientMaskUp(std::vector<unsigned char>& gradientData) const {
	const unsigned char MAX_VALUE = 255;

	for (int y = 0; y < height; y++) {
		unsigned char gradientValue = MAX_VALUE - static_cast<unsigned char>(std::round((y / static_cast<float>(height)) * MAX_VALUE));
		int indexRowStartIndex = (y * width);

		for (int x = 0; x < width; x++) {
			int index = indexRowStartIndex + x;
			gradientData[index] = gradientValue;
		}
	}
}

void pcb::ProceduralHeightmapOperationGradient::generateLinearGradientMaskDown(std::vector<unsigned char>& gradientData) const {
	const unsigned char MAX_VALUE = 255;

	for (int y = 0; y < height; y++) {
		unsigned char gradientValue = static_cast<unsigned char>(std::round((y / static_cast<float>(height)) * MAX_VALUE));
		int indexRowStartIndex = (y * width);

		for (int x = 0; x < width; x++) {
			int index = indexRowStartIndex + x;
			gradientData[index] = gradientValue;
		}
	}
}

void pcb::ProceduralHeightmapOperationGradient::generateLinearGradientMaskLeft(std::vector<unsigned char>& gradientData) const {
	const unsigned char MAX_VALUE = 255;

	for (int x = 0; x < width; x++) {
		unsigned char gradientValue = static_cast<unsigned char>(std::round((x / static_cast<float>(width)) * MAX_VALUE));

		for (int y = 0; y < height; y++) {
			int index = (y * width) + x;
			gradientData[index] = gradientValue;
		}
	}
}

void pcb::ProceduralHeightmapOperationGradient::generateLinearGradientMaskRight(std::vector<unsigned char>& gradientData) const {
	const unsigned char MAX_VALUE = 255;

	for (int x = 0; x < width; x++) {
		unsigned char gradientValue = MAX_VALUE - static_cast<unsigned char>(std::round((x / static_cast<float>(width)) * MAX_VALUE));

		for (int y = 0; y < height; y++) {
			int index = (y * width) + x;
			gradientData[index] = gradientValue;
		}
	}
}