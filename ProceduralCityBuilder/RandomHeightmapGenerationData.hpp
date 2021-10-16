#pragma once

#include <memory>
#include <vector>

#include "BoundsPair.hpp"
#include "LayerData.hpp"

namespace pcb {
	class RandomHeightmapGenerationData final {
	public:
		std::unique_ptr<std::vector<std::unique_ptr<LayerData>>> layerData;

		BoundsPair<unsigned int> adjustmentLoweringThresholds;
		unsigned int adjustmentLoweringValue;
		unsigned int adjustmentScaleDownAmplitudeThreshold;
		double adjustmentScaleDownAmplitudeValue;
		unsigned int adjustmentScaleUpAmplitudeThreshold;
		double adjustmentScaleUpAmplitudeValue;
	};
}