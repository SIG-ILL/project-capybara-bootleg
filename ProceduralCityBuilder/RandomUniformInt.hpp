#pragma once

#include <random>

namespace pcb {
	class RandomUniformInt final {
	public:
		RandomUniformInt(int min, int max);

		int generate();

	private:
		std::mt19937 generator;
		std::uniform_int_distribution<> distribution;
	};
}