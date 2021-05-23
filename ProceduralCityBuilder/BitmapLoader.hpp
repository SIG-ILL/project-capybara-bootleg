#pragma once

#include <string>

#include "Image.hpp"

namespace pcb {
	class BitmapLoader final {
	public:
		Image* loadFromFile(std::string filename) const;
	};
}