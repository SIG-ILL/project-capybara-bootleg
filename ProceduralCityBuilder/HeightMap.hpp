#pragma once

#include <vector>
#include <memory>

#include "Image.hpp"

namespace pcb {
	/*
	8-bit greyscale heightmap.
	Elevation data is stored in a single array containing consecutive rows of elevation values.
	*/
	class Heightmap {
	public:
		Heightmap(const int width, const int height);
		Heightmap(const int width, const int height, std::shared_ptr<std::vector<unsigned char>> elevationValues);
		Heightmap(const Image& image);

		Heightmap& operator+=(const Heightmap& other);
		Heightmap& operator-=(const Heightmap& other);

		std::unique_ptr<Image> to24BitImage() const;
		unsigned char getValueAt(int x, int y) const;
		int getWidth() const;
		int getHeight() const;
		int getLowestElevation() const;
		int getHighestElevation() const;

		void add(const Heightmap& other);
		void subtract(const Heightmap& other);
		void raiseToLevel(const unsigned char level);
		void lowerToLevel(const unsigned char level);
		void clamp(const unsigned char lowerLevel, const unsigned char upperLevel);
		void mask(const Heightmap& mask);		// Normalized multiplication based on mask.
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
		std::shared_ptr<std::vector<unsigned char>> elevationValues;
	};
}