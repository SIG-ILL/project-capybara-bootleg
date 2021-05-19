#pragma once

#include "Image.hpp"

namespace pcb {
	/*
	8-bit greyscale heightmap.
	Elevation data is stored in a single array containing consecutive rows of elevation values.
	*/
	class Heightmap {
	public:
		Heightmap(int width, int height, unsigned char* elevationValues);
		Heightmap(const pcb::Heightmap& other);
		~Heightmap();

		pcb::Heightmap& operator+=(const pcb::Heightmap& other);
		pcb::Heightmap& operator-=(const pcb::Heightmap& other);

		pcb::Image* to24BitImageNew();
		unsigned char getValueAt(int x, int y);
		int getWidth();
		int getHeight();

	private:
		int width;
		int height;
		unsigned char* elevationValues;
	};
}