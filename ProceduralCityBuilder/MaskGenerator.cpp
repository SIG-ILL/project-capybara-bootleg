#include "MaskGenerator.hpp"

#include <cmath>
#include <algorithm>
#include <stdexcept>

pcb::Heightmap pcb::MaskGenerator::generateCircleLinearFalloffMask(int width, int height, int unaffectedCircleRadiusInPixels, int falloffWidthInPixels, int offsetX, int offsetY) const {
	if (width < 0 || height < 0 || unaffectedCircleRadiusInPixels < 0 || falloffWidthInPixels < 0) {
		throw std::invalid_argument("An argument has a negative value. Only zero and positive values are allowed.");
	}

	unsigned char* maskData = new unsigned char[width * height];

	const float centerX = (width - 1) / 2.0f;
	const float centerY = (height - 1) / 2.0f;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			unsigned char value = MAX_MASK_VALUE;
			float horizontalDistance = std::abs(x - centerX) + (width % 2 == 1 ? 0.5f : 0.0f);
			float verticalDistance = std::abs(y - centerY) + (height % 2 == 1 ? 0.5f : 0.0f);
			float distanceToCenter = std::sqrt((horizontalDistance * horizontalDistance) + (verticalDistance * verticalDistance));

			if (distanceToCenter >= unaffectedCircleRadiusInPixels) {
				float progressIntoFalloffArea = std::min(((distanceToCenter - unaffectedCircleRadiusInPixels) / falloffWidthInPixels), 1.0f);
				value = static_cast<unsigned char>(std::round(MAX_MASK_VALUE - (progressIntoFalloffArea * MAX_MASK_VALUE)));
			}

			maskData[(y * width) + x] = value;
		}
	}

	unsigned char* offsettedData = new unsigned char[width * height];
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (y - offsetY < 0 || x - offsetX < 0 || y - offsetY > height - 1 || x - offsetX > width - 1) {
				offsettedData[(y * width) + x] = 0;
			}
			else {
				offsettedData[(y * width) + x] = maskData[((y - offsetY) * width) + (x - offsetX)];
			}			
		}
	}

	pcb::Heightmap mask(width, height, offsettedData);
	delete[] maskData;
	delete[] offsettedData;

	return mask;
}

pcb::Heightmap pcb::MaskGenerator::generateRectangleLinearFalloffMask(int width, int height, int horizontalUnaffectedRadiusInPixels, int verticalUnaffectedRadiusInPixels, int falloffWidthInPixels, int offsetX, int offsetY) const {
	if (width < 0 || height < 0 || horizontalUnaffectedRadiusInPixels < 0 || verticalUnaffectedRadiusInPixels < 0 || falloffWidthInPixels < 0) {
		throw std::invalid_argument("An argument has a negative value. Only zero and positive values are allowed.");
	}

	unsigned char* maskData = new unsigned char[width * height];

	const float centerX = (width - 1) / 2.0f;
	const float centerY = (height - 1) / 2.0f;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			unsigned char horizontalValue = MAX_MASK_VALUE;
			float horizontalDistance = std::abs(x - centerX) + (width % 2 == 1 ? 0.5f : 0.0f);
			if (horizontalDistance >= horizontalUnaffectedRadiusInPixels) {
				float progressIntoFalloffArea = std::min(((horizontalDistance - horizontalUnaffectedRadiusInPixels) / falloffWidthInPixels), 1.0f);
				horizontalValue = static_cast<unsigned char>(std::round(MAX_MASK_VALUE - (progressIntoFalloffArea * MAX_MASK_VALUE)));
			}

			unsigned char verticalValue = MAX_MASK_VALUE;
			float verticalDistance = std::abs(y - centerY) + (height % 2 == 1 ? 0.5f : 0.0f);
			if (verticalDistance >= verticalUnaffectedRadiusInPixels) {
				float progressIntoFalloffArea = std::min(((verticalDistance - verticalUnaffectedRadiusInPixels) / falloffWidthInPixels), 1.0f);
				verticalValue = static_cast<unsigned char>(std::round(MAX_MASK_VALUE - (progressIntoFalloffArea * MAX_MASK_VALUE)));
			}

			maskData[(y * width) + x] = std::min(horizontalValue, verticalValue);
		}
	}

	unsigned char* offsettedData = new unsigned char[width * height];
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (y - offsetY < 0 || x - offsetX < 0 || y - offsetY > height - 1 || x - offsetX > width - 1) {
				offsettedData[(y * width) + x] = 0;
			}
			else {
				offsettedData[(y * width) + x] = maskData[((y - offsetY) * width) + (x - offsetX)];
			}
		}
	}

	pcb::Heightmap mask(width, height, offsettedData);
	delete[] maskData;
	delete[] offsettedData;

	return mask;
}