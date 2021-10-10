#pragma once

#include <string>

#include "Image.hpp"

namespace pcb {
	class BitmapWriter final {
	public:
		void writeToFile(const Image& image, const std::string& filepath) const;
	};
}