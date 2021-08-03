#include "MaskGenerator.hpp"

#include <cmath>
#include <algorithm>
#include <stdexcept>

pcb::Heightmap pcb::MaskGenerator::generateCircleLinearFalloffMask(int width, int height, int unaffectedCircleRadiusInPixels, int falloffWidthInPixels) {
	if (width < 0 || height < 0 || unaffectedCircleRadiusInPixels < 0 || falloffWidthInPixels < 0) {
		throw std::invalid_argument("An argument has a negative value. Only zero and positive values are allowed.");
	}

	unsigned char* maskData = new unsigned char[width * height];

	const float centerX = (width - 1) / 2.0f;
	const float centerY = (height - 1) / 2.0f;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			unsigned char value = 255;
			float horizontalDistance = std::abs(x - centerX) + (width % 2 == 1 ? 0.5f : 0.0f);
			float verticalDistance = std::abs(y - centerY) + (height % 2 == 1 ? 0.5f : 0.0f);
			float distanceToCenter = std::sqrt((horizontalDistance * horizontalDistance) + (verticalDistance * verticalDistance));
			if (distanceToCenter >= unaffectedCircleRadiusInPixels) {
				float progressIntoFalloffArea = std::min(((distanceToCenter - unaffectedCircleRadiusInPixels) / falloffWidthInPixels), 1.0f);
				value = static_cast<unsigned char>(std::round(255 - (progressIntoFalloffArea * 255)));
			}
			maskData[(y * width) + x] = value;
		}
	}

	pcb::Heightmap mask(width, height, maskData);
	delete[] maskData;

	return mask;
}

pcb::Heightmap pcb::MaskGenerator::generateRectangleLinearFalloffMask(int width, int height, int horizontalUnaffectedSizeInPixels, int verticalUnaffectedSizeInPixels, int falloffWidthInPixels) {
	if (width < 0 || height < 0 || horizontalUnaffectedSizeInPixels < 0 || verticalUnaffectedSizeInPixels < 0 || falloffWidthInPixels < 0) {
		throw std::invalid_argument("An argument has a negative value. Only zero and positive values are allowed.");
	}

	unsigned char* maskData = new unsigned char[width * height];

	const float centerX = (width - 1) / 2.0f;
	const float centerY = (height - 1) / 2.0f;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (y == 10) int z = 0;
			unsigned char horizontalValue = 255;
			unsigned char verticalValue = 255;
			float horizontalDistance = std::abs(x - centerX) + (width % 2 == 1 ? 0.5f : 0.0f);
			if (horizontalDistance >= horizontalUnaffectedSizeInPixels) {
				float progressIntoFalloffArea = std::min(((horizontalDistance - horizontalUnaffectedSizeInPixels) / falloffWidthInPixels), 1.0f);
				horizontalValue = static_cast<unsigned char>(std::round(255 - (progressIntoFalloffArea * 255)));
			}

			float verticalDistance = std::abs(y - centerY) + (height % 2 == 1 ? 0.5f : 0.0f);
			if (verticalDistance >= verticalUnaffectedSizeInPixels) {
				float progressIntoFalloffArea = std::min(((verticalDistance - verticalUnaffectedSizeInPixels) / falloffWidthInPixels), 1.0f);
				verticalValue = static_cast<unsigned char>(std::round(255 - (progressIntoFalloffArea * 255)));
			}

			maskData[(y * width) + x] = std::min(horizontalValue, verticalValue);
		}
	}

	pcb::Heightmap mask(width, height, maskData);
	delete[] maskData;

	return mask;
}