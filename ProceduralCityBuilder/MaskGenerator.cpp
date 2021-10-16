#include "MaskGenerator.hpp"

#include <cmath>
#include <algorithm>
#include <stdexcept>

#include "Logger.hpp"
#include "GradientDirection.hpp"

pcb::MaskGenerator::MaskGenerator(int width, int height) : width(width), height(height), absoluteNoiseMapGenerator(width, height), noiseMapGenerator(width, height) {}

std::unique_ptr<pcb::Heightmap> pcb::MaskGenerator::generateCircleLinearFalloffMask(int unaffectedCircleRadiusInPixels, int falloffWidthInPixels, int offsetX, int offsetY) const {
	if (width < 0 || height < 0 || unaffectedCircleRadiusInPixels < 0 || falloffWidthInPixels < 0) {
		throw std::invalid_argument("An argument has a negative value. Only zero and positive values are allowed.");
	}

	std::vector<unsigned char> maskData(width * height, 0);
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

			int index = (y * width) + x;
			maskData[index] = value;
		}
	}

	std::unique_ptr<std::vector<unsigned char>> offsettedData = std::make_unique<std::vector<unsigned char>>(maskData.size(), 0);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int index = (y * width) + x;
			if (y - offsetY < 0 || x - offsetX < 0 || y - offsetY > height - 1 || x - offsetX > width - 1) {
				(*offsettedData)[index] = 0;
			}
			else {
				(*offsettedData)[index] = maskData.at(((y - offsetY) * width) + (x - offsetX));
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

			int index = (y * width) + x;
			maskData[index] = std::min(horizontalValue, verticalValue);
		}
	}

	std::unique_ptr<std::vector<unsigned char>> offsettedData = std::make_unique<std::vector<unsigned char>>(maskData.size(), 0);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int index = (y * width) + x;
			if (y - offsetY < 0 || x - offsetX < 0 || y - offsetY > height - 1 || x - offsetX > width - 1) {
				(*offsettedData)[index] = 0;
			}
			else {
				(*offsettedData)[index] = maskData.at(((y - offsetY) * width) + (x - offsetX));
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

	for (int y = 0; y < height; y++) {
		unsigned char gradientValue;
		if (direction == GradientDirection::Up) {
			gradientValue = MAX_MASK_VALUE - static_cast<unsigned char>(std::round((y / static_cast<float>(height)) * MAX_MASK_VALUE));			
		}
		else if (direction == GradientDirection::Down) {
			gradientValue = static_cast<unsigned char>(std::round((y / static_cast<float>(height)) * MAX_MASK_VALUE));
		}	

		for (int x = 0; x < width; x++) {
			if (direction == GradientDirection::Left) {
				gradientValue = static_cast<unsigned char>(std::round((x / static_cast<float>(width)) * MAX_MASK_VALUE));
			}
			else if (direction == GradientDirection::Right) {
				gradientValue = MAX_MASK_VALUE - static_cast<unsigned char>(std::round((x / static_cast<float>(width)) * MAX_MASK_VALUE));
			}

			int index = (y * width) + x;
			(*maskData)[index] = gradientValue;
		}
	}

	return std::make_unique<Heightmap>(width, height, std::move(maskData));
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
		else if (layerParameters.maskType == MaskType::LinearGradient) {		// TODO: If there already is a linear gradient in the vector, don't add the gradient that goes the opposite way as they will cancel each other out and producte a mask that doesn't do anything (or masks *everything* if it's inverted).
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
		mask->add(*(maskLayers.at(i)));
	}

	if (parameters.invert) {
		logger << "Inverting mask!\n";
		mask->invert();
	}

	return mask;
}