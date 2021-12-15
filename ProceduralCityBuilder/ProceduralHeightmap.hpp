#pragma once

#include <string>

#include "ProceduralHeightmapOperation.hpp"

namespace pcb {
	class ProceduralHeightmap final {
	public:
		ProceduralHeightmap(std::unique_ptr<ProceduralHeightmapOperation> topLevelOperation);
		ProceduralHeightmap(const std::string& stringDefinition);

		std::unique_ptr<Heightmap> generateResult();

	private:
		std::unique_ptr<ProceduralHeightmapOperation> topLevelOperation;
	};
}