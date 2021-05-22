#pragma once

#include "Image.hpp"

namespace pcb {
	/*
	8-bit greyscale heightmap.
	Elevation data is stored in a single array containing consecutive rows of elevation values.
	*/
	class Heightmap {
	public:
		Heightmap(const int width, const int height, const unsigned char* elevationValues);
		Heightmap(const pcb::Heightmap& other);
		virtual ~Heightmap();

		pcb::Heightmap& operator+=(const pcb::Heightmap& other);
		pcb::Heightmap& operator-=(const pcb::Heightmap& other);

		pcb::Image* to24BitImageNew() const;
		unsigned char getValueAt(int x, int y) const;
		virtual int getWidth() const;
		virtual int getHeight() const;
		int getHighestElevation() const;

	protected:
		void add(const pcb::Heightmap& other);
		void subtract(const pcb::Heightmap& other);

	private:
		int width;
		int height;
		unsigned char highestElevation;
		unsigned char* elevationValues;
	};
}