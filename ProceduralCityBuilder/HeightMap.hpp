#pragma once

#include <vector>

#include "Image.hpp"

namespace pcb {
	/*
	8-bit greyscale heightmap.
	Elevation data is stored in a single array containing consecutive rows of elevation values.
	*/
	class Heightmap {
	public:
		Heightmap(const int width, const int height);
		Heightmap(const int width, const int height, const std::vector<unsigned char>& elevationValues);

		Heightmap& operator+=(const Heightmap& other);
		Heightmap& operator-=(const Heightmap& other);

		pcb::Image to24BitImage() const;
		unsigned char getValueAt(int x, int y) const;
		int getWidth() const;
		int getHeight() const;
		int getLowestElevation() const;
		int getHighestElevation() const;

		void add(const pcb::Heightmap& other);
		void subtract(const pcb::Heightmap& other);
		void raiseToLevel(const unsigned char level);
		void lowerToLevel(const unsigned char level);
		void clamp(const unsigned char lowerLevel, const unsigned char upperLevel);
		void mask(const pcb::Heightmap& mask);		// Normalized multiplication based on mask.
		void scale(const double factor);
		void invert();
		void scaleAmplitude(const double factor);	// Scales amplitude measured from current (lowest + (0.5 * (highest - lowest))) elevation value
		void raise(const unsigned char amount);
		void lower(const unsigned char amount);

	private:
		static const unsigned char MIN_ELEVATION_VALUE = 0;
		static const unsigned char MAX_ELEVATION_VALUE = 255;

		int width;
		int height;
		unsigned char lowestElevation;
		unsigned char highestElevation;
		std::vector<unsigned char> elevationValues;
	};
}