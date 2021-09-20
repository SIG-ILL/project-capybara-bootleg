#pragma once

#include <random>

#include "BoundsPair.hpp"

namespace pcb {
	class RandomUniformReal final {
	public:
		RandomUniformReal(double min, double max);
		RandomUniformReal(BoundsPair<double> bounds);

		double generate();

	private:
		std::mt19937 generator;
		std::uniform_real_distribution<> distribution;
	};
}