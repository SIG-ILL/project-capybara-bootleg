#include "MaskGenerator.hpp"

#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <chrono>

#include "Logger.hpp"
#include "GradientDirection.hpp"

pcb::MaskGenerator::MaskGenerator(int width, int height) : width(width), height(height), absoluteNoiseMapGenerator(width, height), noiseMapGenerator(width, height) {}

std::unique_ptr<pcb::Heightmap> pcb::MaskGenerator::generateCircleLinearFalloffMask(int unaffectedCircleRadiusInPixels, int falloffWidthInPixels, int offsetX, int offsetY) const {
	if (width < 0 || height < 0 || unaffectedCircleRadiusInPixels < 0 || falloffWidthInPixels < 0) {
		throw std::invalid_argument("An argument has a negative value. Only zero and positive values are allowed.");
	}

	std::vector<unsigned char> maskData(width * height, 0);
	const float CENTER_X = (width - 1) / 2.0f;
	const float CENTER_Y = (height - 1) / 2.0f;

	for (int y = 0; y < height; y++) {
		int indexRowStartIndex = (y * width);
		float verticalDistance = std::abs(y - CENTER_Y) + (height % 2 == 1 ? 0.5f : 0.0f);
		float verticalDistanceSquared = (verticalDistance * verticalDistance);

		for (int x = 0; x < width; x++) {
			unsigned char value = MAX_MASK_VALUE;
			float horizontalDistance = std::abs(x - CENTER_X) + (width % 2 == 1 ? 0.5f : 0.0f);			
			float distanceToCenter = std::sqrt((horizontalDistance * horizontalDistance) + verticalDistanceSquared);

			if (distanceToCenter >= unaffectedCircleRadiusInPixels) {
				float progressIntoFalloffArea = std::min(((distanceToCenter - unaffectedCircleRadiusInPixels) / falloffWidthInPixels), 1.0f);
				value = static_cast<unsigned char>(std::round(MAX_MASK_VALUE - (progressIntoFalloffArea * MAX_MASK_VALUE)));
			}

			int index = indexRowStartIndex + x;
			maskData[index] = value;
		}
	}

	std::unique_ptr<std::vector<unsigned char>> offsettedData = std::make_unique<std::vector<unsigned char>>(maskData.size(), 0);
	for (int y = 0; y < height; y++) {
		int indexRowStartIndex = (y * width);
		int offsettedY = (y - offsetY);
		int offsettedIndexRowStartIndex = (offsettedY * width);

		for (int x = 0; x < width; x++) {
			int index = indexRowStartIndex + x;
			int offsettedX = x - offsetX;

			if (offsettedY < 0 || offsettedX < 0 || offsettedY > height - 1 || offsettedX > width - 1) {
				(*offsettedData)[index] = 0;
			}
			else {
				(*offsettedData)[index] = maskData.at(offsettedIndexRowStartIndex + offsettedX);
			}			
		}
	}

	return std::make_unique<Heightmap>(width, height, std::move(offsettedData));
}

std::unique_ptr<pcb::Heightmap> pcb::MaskGenerator::generateRectangleLinearFalloffMask(int horizontalUnaffectedRadiusInPixels, int verticalUnaffectedRadiusInPixels, int falloffWidthInPixels, int offsetX, int offsetY) const {
	if (width < 0 || height < 0 || horizontalUnaffectedRadiusInPixels < 0 || verticalUnaffectedRadiusInPixels < 0 || falloffWidthInPixels < 0) {
		throw std::invalid_argument("An argument has a negative value. Only zero and positive values are allowed.");
	}

	std::vector<unsigned char> maskData(width * height, 0);

	const float CENTER_X = (width - 1) / 2.0f;
	const float CENTER_Y = (height - 1) / 2.0f;

	for (int y = 0; y < height; y++) {
		int indexRowStartIndex = (y * width);

		float verticalDistance = std::abs(y - CENTER_Y) + (height % 2 == 1 ? 0.5f : 0.0f);
		float verticalProgressIntoFalloffArea = std::min(((verticalDistance - verticalUnaffectedRadiusInPixels) / falloffWidthInPixels), 1.0f);
		unsigned char verticalValue = MAX_MASK_VALUE;
		if (verticalDistance >= verticalUnaffectedRadiusInPixels) {
			verticalValue = static_cast<unsigned char>(std::round(MAX_MASK_VALUE - (verticalProgressIntoFalloffArea * MAX_MASK_VALUE)));
		}

		for (int x = 0; x < width; x++) {
			unsigned char horizontalValue = MAX_MASK_VALUE;
			float horizontalDistance = std::abs(x - CENTER_X) + (width % 2 == 1 ? 0.5f : 0.0f);
			if (horizontalDistance >= horizontalUnaffectedRadiusInPixels) {
				float progressIntoFalloffArea = std::min(((horizontalDistance - horizontalUnaffectedRadiusInPixels) / falloffWidthInPixels), 1.0f);
				horizontalValue = static_cast<unsigned char>(std::round(MAX_MASK_VALUE - (progressIntoFalloffArea * MAX_MASK_VALUE)));
			}

			int index = indexRowStartIndex + x;
			maskData[index] = std::min(horizontalValue, verticalValue);
		}
	}

	std::unique_ptr<std::vector<unsigned char>> offsettedData = std::make_unique<std::vector<unsigned char>>(maskData.size(), 0);
	for (int y = 0; y < height; y++) {
		int indexRowStartIndex = (y * width);
		int offsettedY = (y - offsetY);
		int offsettedIndexRowStartIndex = (offsettedY * width);

		for (int x = 0; x < width; x++) {
			int index = indexRowStartIndex + x;
			int offsettedX = x - offsetX;

			if (offsettedY < 0 || offsettedX < 0 || offsettedY > height - 1 || offsettedX > width - 1) {
				(*offsettedData)[index] = 0;
			}
			else {
				(*offsettedData)[index] = maskData.at(offsettedIndexRowStartIndex + offsettedX);
			}
		}
	}

	return std::make_unique<Heightmap>(width, height, std::move(offsettedData));
}

std::unique_ptr<pcb::Heightmap> pcb::MaskGenerator::generateLinearGradientMask(GradientDirection direction) const {
	if (width < 0 || height < 0) {
		throw std::invalid_argument("An argument has a negative value. Only zero and positive values are allowed.");
	}

	std::unique_ptr<std::vector<unsigned char>> maskData = std::make_unique<std::vector<unsigned char>>(width * height, 0);
	// The different directions have different functions for performace/optimization reasons.
	if (direction == GradientDirection::Down) {
		generateLinearGradientMaskDown(*maskData);
	}
	else if (direction == GradientDirection::Up) {
		generateLinearGradientMaskUp(*maskData);
	}
	else if (direction == GradientDirection::Left) {
		generateLinearGradientMaskLeft(*maskData);
	}
	else {
		generateLinearGradientMaskRight(*maskData);
	}

	return std::make_unique<Heightmap>(width, height, std::move(maskData));
}

void pcb::MaskGenerator::generateLinearGradientMaskUp(std::vector<unsigned char>& maskData) const {
	for (int y = 0; y < height; y++) {
		unsigned char gradientValue = MAX_MASK_VALUE - static_cast<unsigned char>(std::round((y / static_cast<float>(height)) * MAX_MASK_VALUE));
		int indexRowStartIndex = (y * width);

		for (int x = 0; x < width; x++) {
			int index = indexRowStartIndex + x;
			maskData[index] = gradientValue;
		}
	}
}

void pcb::MaskGenerator::generateLinearGradientMaskDown(std::vector<unsigned char>& maskData) const {
	for (int y = 0; y < height; y++) {
		unsigned char gradientValue = static_cast<unsigned char>(std::round((y / static_cast<float>(height)) * MAX_MASK_VALUE));
		int indexRowStartIndex = (y * width);

		for (int x = 0; x < width; x++) {
			int index = indexRowStartIndex + x;
			maskData[index] = gradientValue;
		}
	}
}

void pcb::MaskGenerator::generateLinearGradientMaskLeft(std::vector<unsigned char>& maskData) const {
	for (int x = 0; x < width; x++) {
		unsigned char gradientValue = static_cast<unsigned char>(std::round((x / static_cast<float>(width)) * MAX_MASK_VALUE));

		for (int y = 0; y < height; y++) {
			int index = (y * width) + x;
			maskData[index] = gradientValue;
		}
	}
}

void pcb::MaskGenerator::generateLinearGradientMaskRight(std::vector<unsigned char>& maskData) const {
	for (int x = 0; x < width; x++) {
		unsigned char gradientValue = MAX_MASK_VALUE - static_cast<unsigned char>(std::round((x / static_cast<float>(width)) * MAX_MASK_VALUE));

		for (int y = 0; y < height; y++) {
			int index = (y * width) + x;
			maskData[index] = gradientValue;
		}
	}
}

std::unique_ptr<pcb::Heightmap> pcb::MaskGenerator::generateCombinedMask(const CombinedMaskGenerationParameters& parameters) const {
	Logger logger;
	logger << "Amount of mask layers: " << parameters.layerParameters.size() << "\n";

	std::vector<std::unique_ptr<Heightmap>> maskLayers;
	for (int i = 0; i < parameters.layerParameters.size(); i++) {
		CombinedMaskGenerationLayerParameters layerParameters = parameters.layerParameters.at(i);

		if (layerParameters.maskType == MaskType::Circle) {
			logger << "MaskType: Circle\n";
			maskLayers.push_back(generateCircleLinearFalloffMask(layerParameters.unaffectedRadiusX, layerParameters.falloffWidth, layerParameters.offsetX, layerParameters.offsetY));
		}
		else if (layerParameters.maskType == MaskType::Rectangle) {
			logger << "MaskType: Rectangle\n";
			maskLayers.push_back(generateRectangleLinearFalloffMask(layerParameters.unaffectedRadiusX, layerParameters.unaffectedRadiusY, layerParameters.falloffWidth, layerParameters.offsetX, layerParameters.offsetY));
		}
		else if (layerParameters.maskType == MaskType::LinearGradient) {
			logger << "MaskType: Linear Gradient\n";
			switch (layerParameters.gradientDirection) {
			case GradientDirection::Up:
				logger << "GradientDirection: Up\n";
				break;
			case GradientDirection::Left:
				logger << "GradientDirection: Left\n";
				break;
			case GradientDirection::Down:
				logger << "GradientDirection: Down\n";
				break;
			case GradientDirection::Right:
				logger << "GradientDirection: Right\n";
				break;
			}

			maskLayers.push_back(generateLinearGradientMask(layerParameters.gradientDirection));
		}
		else if (layerParameters.maskType == MaskType::Noise) {
			logger << "MaskType: Noise - ";
			if (layerParameters.isAbsoluteNoise) {
				logger << "Absolute\n";
				maskLayers.push_back(absoluteNoiseMapGenerator.generate(layerParameters.noiseFrequencyX, layerParameters.noiseFrequencyY, layerParameters.offsetX, layerParameters.offsetY));
			}
			else {
				logger << "Default\n";
				maskLayers.push_back(noiseMapGenerator.generate(layerParameters.noiseFrequencyX, layerParameters.noiseFrequencyY, layerParameters.offsetX, layerParameters.offsetY));
			}

			if (layerParameters.invert) {
				logger << "Inverting mask noise layer\n";
				maskLayers.back()->invert();
			}
		}
	}

	std::unique_ptr<Heightmap> mask = std::move(maskLayers.front());
	maskLayers.erase(maskLayers.begin());
	for (int i = 0; i < maskLayers.size(); i++) {
		mask->add(*(maskLayers[i]));
	}

	if (parameters.invert) {
		logger << "Inverting mask!\n";
		mask->invert();
	}

	return mask;
}