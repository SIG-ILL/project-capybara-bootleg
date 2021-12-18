#pragma once

#include "ProceduralHeightmapOperation.hpp"

namespace pcb {
	class ProceduralHeightmapOperationEmptyMap final : public ProceduralHeightmapOperation {
	public:
		ProceduralHeightmapOperationEmptyMap(int width, int height);

		std::unique_ptr<Heightmap> generateResult() const override;
		int getHeightmapWidth() const override;
		int getHeightmapHeight() const override;

	private:
		int width;
		int height;
	};
}