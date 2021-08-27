#pragma once

#include <random>

namespace pcb {
	class RandomUniformReal final {
	public:
		RandomUniformReal(double min, double max);

		double generate();

	private:
		std::mt19937 generator;
		std::uniform_real_distribution<> distribution;
	};
}