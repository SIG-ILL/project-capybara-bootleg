#pragma once

#include <vector>
#include <memory>

#include "HeightmapLayer.hpp"
#include "HeightmapGenerationProperties.hpp"
#include "LayerData.hpp"

namespace pcb {
	class RandomHeightmapLayerDataGenerator final {
	public:
		RandomHeightmapLayerDataGenerator();
		RandomHeightmapLayerDataGenerator(const RandomGenerationControlParameters& controlParameters);

		std::unique_ptr<std::vector<std::unique_ptr<LayerData>>> generateLayerData(int width, int height) const;
		RandomGenerationControlParameters getDefaultRandomGenerationControlParameters() const;
		void setRandomGenerationControlParameters(const RandomGenerationControlParameters& controlParameters);

	private:
		RandomGenerationControlParameters controlParameters;
	};
}