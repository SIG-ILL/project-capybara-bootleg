#include "ProceduralHeightmapOperationCircle.hpp"

#include <stdexcept>
#include <cmath>

pcb::ProceduralHeightmapOperationCircle::ProceduralHeightmapOperationCircle(int width, int height, int unaffectedCircleRadiusInPixels, int falloffWidthInPixels, int offsetX, int offsetY)
	: width(width), height(height), unaffectedCircleRadiusInPixels(unaffectedCircleRadiusInPixels), falloffWidthInPixels(falloffWidthInPixels), offsetX(offsetX), offsetY(offsetY) {}

std::unique_ptr<pcb::Heightmap> pcb::ProceduralHeightmapOperationCircle::generateResult() const {
	if (width < 0 || height < 0 || unaffectedCircleRadiusInPixels < 0 || falloffWidthInPixels < 0) {
		throw std::invalid_argument("An argument has a negative value. Only zero and positive values are allowed.");
	}

	std::unique_ptr<std::vector<unsigned char>> circleData = std::make_unique<std::vector<unsigned char>>(width * height, 0);
	const unsigned char MAX_VALUE = 255;
	const float CENTER_X = ((width - 1) / 2.0f) + offsetX;
	const float CENTER_Y = ((height - 1) / 2.0f) + offsetY;
	const float VERTICAL_DISTANCE_ADDITION = (height % 2 == 1 ? 0.5f : 0.0f);
	const float HORIZONTAL_DISTANCE_ADDITION = (width % 2 == 1 ? 0.5f : 0.0f);

	int circleTotalRadius = unaffectedCircleRadiusInPixels + falloffWidthInPixels;
	int minX = static_cast<int>(std::round(std::max((CENTER_X - circleTotalRadius), 0.0f)));
	int maxX = static_cast<int>(std::round(std::min((CENTER_X + circleTotalRadius), static_cast<float>(width))));
	int minY = static_cast<int>(std::round(std::max((CENTER_Y - circleTotalRadius), 0.0f)));
	int maxY = static_cast<int>(std::round(std::min((CENTER_Y + circleTotalRadius), static_cast<float>(height))));

	for (int y = minY; y < maxY; y++) {
		int indexRowStartIndex = (y * width);
		float verticalDistance = std::abs(y - CENTER_Y) + VERTICAL_DISTANCE_ADDITION;
		float verticalDistanceSquared = (verticalDistance * verticalDistance);

		for (int x = minX; x < maxX; x++) {
			float horizontalDistance = std::abs(x - CENTER_X) + HORIZONTAL_DISTANCE_ADDITION;
			float distanceToCenter = std::sqrt((horizontalDistance * horizontalDistance) + verticalDistanceSquared);
			unsigned char value = MAX_VALUE;

			if (distanceToCenter >= unaffectedCircleRadiusInPixels) {
				float progressIntoFalloffArea = std::min(((distanceToCenter - unaffectedCircleRadiusInPixels) / falloffWidthInPixels), 1.0f);
				value = static_cast<unsigned char>(std::round(MAX_VALUE - (progressIntoFalloffArea * MAX_VALUE)));
			}

			int index = indexRowStartIndex + x;
			(*circleData)[index] = value;
		}
	}

	return std::make_unique<Heightmap>(width, height, std::move(circleData));
}

int pcb::ProceduralHeightmapOperationCircle::getHeightmapWidth() const {
	return width;
}

int pcb::ProceduralHeightmapOperationCircle::getHeightmapHeight() const {
	return height;
}