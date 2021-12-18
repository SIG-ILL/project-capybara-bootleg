#pragma once

#include "ProceduralHeightmapOperation.hpp"

namespace pcb {
	class ProceduralHeightmapOperationScaleAmplitude final : public ProceduralHeightmapOperation {
	public:
		ProceduralHeightmapOperationScaleAmplitude(std::shared_ptr<ProceduralHeightmapOperation> sourceToModify, const double factor);

		std::unique_ptr<Heightmap> generateResult() const override;
		int getHeightmapWidth() const override;
		int getHeightmapHeight() const override;

	private:
		std::shared_ptr<ProceduralHeightmapOperation> sourceToModify;
		double factor;
	};
}