#pragma once

#include "Image.hpp"

namespace pcb {
	/*
	8-bit greyscale heightmap.
	Elevation data is stored in a single array containing consecutive rows of elevation values.
	*/
	class HeightMap {
	public:
		HeightMap(int width, int height, unsigned char* elevationValues);
		~HeightMap();

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