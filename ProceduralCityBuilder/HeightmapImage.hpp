#pragma once

#include <vector>
#include <memory>

#include "Image.hpp"

namespace pcb {
	class HeightmapImage final {
	public:
		std::shared_ptr<Image> finalImage;
		std::vector<std::shared_ptr<Image>> layerImages;
	};
}