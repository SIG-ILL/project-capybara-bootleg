#pragma once

#include <array>

namespace pcb {
	class Gradient final {
	public:
		Gradient(double x, double y) : x(x), y(y) {};

		double x;
		double y;
	};

	/*
	* Not-quiet simplex noise implementation taken from Stefan Gustavson - Simplex noise demystified (https://weber.itn.liu.se/~stegu/simplexnoise/simplexnoise.pdf with updated code http://www.itn.liu.se/~stegu/simplexnoise/SimplexNoise.java)
	*/
	class NoiseGenerator final {
	public:
		NoiseGenerator();

		/*
		* Returns a value [-1; 1] for the given coordinate.
		*/
		double getValueForCoordinate(double x, double y) const;

	private:
		const double F;
		const double G;

		std::array<Gradient, 16> grad;
		std::array<short, 512> perm;
		std::array<short, 512> permMod16;

		double dot(const Gradient& g, double x, double y) const;
	};
}