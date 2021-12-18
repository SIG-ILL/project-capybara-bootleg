#include "ProceduralHeightmapOperationRectangle.hpp"

#include <stdexcept>
#include <cmath>

pcb::ProceduralHeightmapOperationRectangle::ProceduralHeightmapOperationRectangle(int width, int height, int horizontalUnaffectedRadiusInPixels, int verticalUnaffectedRadiusInPixels, int horizontalFalloffWidthInPixels, int verticalFalloffWidthInPixels, int offsetX, int offsetY)
	: width(width), height(height), horizontalUnaffectedRadiusInPixels(horizontalUnaffectedRadiusInPixels), verticalUnaffectedRadiusInPixels(verticalUnaffectedRadiusInPixels), horizontalFalloffWidthInPixels(horizontalFalloffWidthInPixels), verticalFalloffWidthInPixels(verticalFalloffWidthInPixels),
	offsetX(offsetX), offsetY(offsetY) {}

std::unique_ptr<pcb::Heightmap> pcb::ProceduralHeightmapOperationRectangle::generateResult() const {
	if (width < 0 || height < 0 || horizontalUnaffectedRadiusInPixels < 0 || verticalUnaffectedRadiusInPixels < 0 || horizontalFalloffWidthInPixels < 0 || verticalFalloffWidthInPixels < 0) {
		throw std::invalid_argument("An argument has a negative value. Only zero and positive values are allowed.");
	}

	std::unique_ptr<std::vector<unsigned char>> rectangleData = std::make_unique<std::vector<unsigned char>>(width * height, 0);
	const unsigned char MAX_VALUE = 255;
	const float CENTER_X = ((width - 1) / 2.0f) + offsetX;
	const float CENTER_Y = ((height - 1) / 2.0f) + offsetY;
	const float VERTICAL_DISTANCE_ADDITION = (height % 2 == 1 ? 0.5f : 0.0f);
	const float HORIZONTAL_DISTANCE_ADDITION = (width % 2 == 1 ? 0.5f : 0.0f);

	int rectangleTotalHorizontalRadius = horizontalUnaffectedRadiusInPixels + horizontalFalloffWidthInPixels;
	int rectangleTotalVerticalRadius = verticalUnaffectedRadiusInPixels + verticalFalloffWidthInPixels;
	int minX = static_cast<int>(std::round(std::max((CENTER_X - rectangleTotalHorizontalRadius), 0.0f)));
	int maxX = static_cast<int>(std::round(std::min((CENTER_X + rectangleTotalHorizontalRadius), static_cast<float>(width))));
	int minY = static_cast<int>(std::round(std::max((CENTER_Y - rectangleTotalVerticalRadius), 0.0f)));
	int maxY = static_cast<int>(std::round(std::min((CENTER_Y + rectangleTotalVerticalRadius), static_cast<float>(height))));

	for (int y = minY; y < maxY; y++) {
		int indexRowStartIndex = (y * width);
		float verticalDistance = std::abs(y - CENTER_Y) + VERTICAL_DISTANCE_ADDITION;
		float verticalProgressIntoFalloffArea = std::min(((verticalDistance - verticalUnaffectedRadiusInPixels) / verticalFalloffWidthInPixels), 1.0f);
		unsigned char verticalValue = MAX_VALUE;

		if (verticalDistance >= verticalUnaffectedRadiusInPixels) {
			verticalValue = static_cast<unsigned char>(std::round(MAX_VALUE - (verticalProgressIntoFalloffArea * MAX_VALUE)));
		}

		for (int x = minX; x < maxX; x++) {
			unsigned char horizontalValue = MAX_VALUE;
			float horizontalDistance = std::abs(x - CENTER_X) + HORIZONTAL_DISTANCE_ADDITION;
			if (horizontalDistance >= horizontalUnaffectedRadiusInPixels) {
				float progressIntoFalloffArea = std::min(((horizontalDistance - horizontalUnaffectedRadiusInPixels) / horizontalFalloffWidthInPixels), 1.0f);
				horizontalValue = static_cast<unsigned char>(std::round(MAX_VALUE - (progressIntoFalloffArea * MAX_VALUE)));
			}

			int index = indexRowStartIndex + x;
			(*rectangleData)[index] = std::min(horizontalValue, verticalValue);
		}
	}

	return std::make_unique<Heightmap>(width, height, std::move(rectangleData));
}

int pcb::ProceduralHeightmapOperationRectangle::getHeightmapWidth() const {
	return width;
}

int pcb::ProceduralHeightmapOperationRectangle::getHeightmapHeight() const {
	return height;
}