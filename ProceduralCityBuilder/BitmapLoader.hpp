#pragma once

#include <string>

#include "Image.hpp"

namespace pcb {
	class BitmapLoader {
	public:
		Image* loadFromFile(std::string filename);
	};
}