#include "ProceduralHeightmapOperationEllipse.hpp"

#include <stdexcept>
#include <cmath>

pcb::ProceduralHeightmapOperationEllipse::ProceduralHeightmapOperationEllipse(int width, int height, int unaffectedHorizontalRadiusInPixels, int unaffectedVerticalRadiusInPixels, float falloffEllipseScaleFactor, int offsetX, int offsetY)
	: width(width), height(height), unaffectedHorizontalRadiusInPixels(unaffectedHorizontalRadiusInPixels), unaffectedVerticalRadiusInPixels(unaffectedVerticalRadiusInPixels), falloffEllipseScaleFactor(falloffEllipseScaleFactor), offsetX(offsetX), offsetY(offsetY) {}

std::unique_ptr<pcb::Heightmap> pcb::ProceduralHeightmapOperationEllipse::generateResult() const {
	if (width < 0 || height < 0 || unaffectedHorizontalRadiusInPixels < 0 || unaffectedVerticalRadiusInPixels < 0 || falloffEllipseScaleFactor < 1) {
		throw std::invalid_argument("An argument has a negative value. Only zero and positive values are allowed.");
	}

	std::unique_ptr<std::vector<unsigned char>> ellipseData = std::make_unique<std::vector<unsigned char>>(width * height, 0);
	const unsigned char MAX_VALUE = 255;
	const float CENTER_X = ((width - 1) / 2.0f) + offsetX;
	const float CENTER_Y = ((height - 1) / 2.0f) + offsetY;
	const float VERTICAL_DISTANCE_ADDITION = (height % 2 == 1 ? 0.5f : 0.0f);
	const float HORIZONTAL_DISTANCE_ADDITION = (width % 2 == 1 ? 0.5f : 0.0f);

	int ellipseTotalHorizontalRadius = static_cast<int>(std::round(unaffectedHorizontalRadiusInPixels * falloffEllipseScaleFactor));
	int ellipseTotalVerticalRadius = static_cast<int>(std::round(unaffectedVerticalRadiusInPixels * falloffEllipseScaleFactor));
	int minX = static_cast<int>(std::round(std::max((CENTER_X - ellipseTotalHorizontalRadius), 0.0f)));
	int maxX = static_cast<int>(std::round(std::min((CENTER_X + ellipseTotalHorizontalRadius), static_cast<float>(width))));
	int minY = static_cast<int>(std::round(std::max((CENTER_Y - ellipseTotalVerticalRadius), 0.0f)));
	int maxY = static_cast<int>(std::round(std::min((CENTER_Y + ellipseTotalVerticalRadius), static_cast<float>(height))));

	const int innerHorizontalRadiusSquared = unaffectedHorizontalRadiusInPixels * unaffectedHorizontalRadiusInPixels;
	const int innerVerticalRadiusSquared = unaffectedVerticalRadiusInPixels * unaffectedVerticalRadiusInPixels;
	const int outerHorizontalRadiusSquared = ellipseTotalHorizontalRadius * ellipseTotalHorizontalRadius;
	const int outerVerticalRadiusSquared = ellipseTotalVerticalRadius * ellipseTotalVerticalRadius;

	for (int y = minY; y < maxY; y++) {
		int indexRowStartIndex = (y * width);
		int relativeY = (y - CENTER_Y);
		int relativeYSquared = relativeY * relativeY;

		for (int x = minX; x < maxX; x++) {
			int relativeX = (x - CENTER_X);
			int relativeXSquared = relativeX * relativeX;
			float distanceX = relativeXSquared / static_cast<float>(outerHorizontalRadiusSquared);
			float distanceY = relativeYSquared / static_cast<float>(outerVerticalRadiusSquared);

			if (distanceX + distanceY <= 1) {	// Pixel is in ellipse.
				unsigned char value = MAX_VALUE;

				if ((static_cast<float>(relativeXSquared) / innerHorizontalRadiusSquared) + (static_cast<float>(relativeYSquared) / innerVerticalRadiusSquared) > 1) {	// Pixel is in falloff/fading/gradient area.
					float angleInRadians = std::atan2f(relativeY, relativeX);

					float sinAngle = std::sinf(angleInRadians);
					float sinAngleSquared = sinAngle * sinAngle;
					float cosAngle = std::cosf(angleInRadians);
					float cosAngleSquared = cosAngle * cosAngle;
					float unaffectedRadiusAtAngle = (unaffectedHorizontalRadiusInPixels * unaffectedVerticalRadiusInPixels) / std::sqrtf((innerHorizontalRadiusSquared * sinAngleSquared) + (innerVerticalRadiusSquared * cosAngleSquared));
					float totalRadiusAtAngle = (ellipseTotalHorizontalRadius * ellipseTotalVerticalRadius) / std::sqrtf((outerHorizontalRadiusSquared* sinAngleSquared) + (outerVerticalRadiusSquared * cosAngleSquared));

					float innerEllipseIntersectionX = unaffectedRadiusAtAngle * cosAngle;
					float innerEllipseIntersectionY = unaffectedRadiusAtAngle * sinAngle;
					float outerEllipseIntersectionX = totalRadiusAtAngle * cosAngle;
					float outerEllipseIntersectionY = totalRadiusAtAngle * sinAngle;
					float distanceBetweenInnerAndOuterEllipseX = outerEllipseIntersectionX - innerEllipseIntersectionX;
					float distanceBetweenInnerAndOuterEllipseY = outerEllipseIntersectionY - innerEllipseIntersectionY;
					float distanceBetweenInnerAndOuterEllipse = std::sqrtf((distanceBetweenInnerAndOuterEllipseX * distanceBetweenInnerAndOuterEllipseX) + (distanceBetweenInnerAndOuterEllipseY * distanceBetweenInnerAndOuterEllipseY));
					float distanceBetweenInnerEllipseAndPointX = relativeX - innerEllipseIntersectionX;
					float distanceBetweenInnerEllipseAndPointY = relativeY - innerEllipseIntersectionY;
					float distanceBetweenInnerEllipseAndPoint = std::sqrtf((distanceBetweenInnerEllipseAndPointX * distanceBetweenInnerEllipseAndPointX) + (distanceBetweenInnerEllipseAndPointY * distanceBetweenInnerEllipseAndPointY));

					float progressIntoFalloffArea = distanceBetweenInnerEllipseAndPoint / distanceBetweenInnerAndOuterEllipse;
					
					value = static_cast<unsigned char>(std::round(MAX_VALUE - (progressIntoFalloffArea * MAX_VALUE)));
				}

				int index = indexRowStartIndex + x;
				(*ellipseData)[index] = value;
			}
		}
	}

	return std::make_unique<Heightmap>(width, height, std::move(ellipseData));
}

int pcb::ProceduralHeightmapOperationEllipse::getHeightmapWidth() const {
	return width;
}

int pcb::ProceduralHeightmapOperationEllipse::getHeightmapHeight() const {
	return height;
}