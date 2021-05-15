#pragma once

#include "Image.hpp"

namespace pcb {
	class TerrainGenerator {
	public:
		void generate();
	};	

	/*
	8-bit greyscale heightmap.
	Elevation data is stored in a single array containing consecutive rows of elevation values.
	*/
	class HeightMap {
	public:
		HeightMap(unsigned int width, unsigned int height, unsigned char* elevationValues);
		~HeightMap();

		void print();
		Image* to24BitImage();

	private:
		unsigned int width;
		unsigned int height;
		unsigned char* elevationValues;
	};

	class HeightMapGenerator {
	public:
		pcb::HeightMap* generate();
	};
}