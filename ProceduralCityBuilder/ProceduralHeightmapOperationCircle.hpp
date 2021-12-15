#pragma once

#include "ProceduralHeightmapOperation.hpp"

namespace pcb {
	class ProceduralHeightmapOperationCircle final : public ProceduralHeightmapOperation {
	public:
		ProceduralHeightmapOperationCircle(int width, int height, int unaffectedCircleRadiusInPixels, int falloffWidthInPixels, int offsetX, int offsetY);

		std::unique_ptr<Heightmap> generateResult() const override;

	private:
		int width;
		int height;
		int unaffectedCircleRadiusInPixels;
		int falloffWidthInPixels;
		int offsetX;
		int offsetY;
	};
}