#pragma once

#include <string>
#include <memory>

#include "Image.hpp"

namespace pcb {
	class BitmapLoader final {
	public:
		std::unique_ptr<Image> loadFromFile(const std::string& filepath) const;
	};
}