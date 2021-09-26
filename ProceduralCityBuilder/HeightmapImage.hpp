#pragma once

#include <vector>

#include "Image.hpp"

namespace pcb {
	class HeightmapImage final {
	public:
		Image finalImage;
		std::vector<Image> layerImages;
	};
}