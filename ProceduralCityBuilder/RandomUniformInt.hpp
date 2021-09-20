#pragma once

#include <random>

#include "BoundsPair.hpp"

namespace pcb {
	class RandomUniformInt final {
	public:
		RandomUniformInt(int min, int max);
		RandomUniformInt(BoundsPair<int> bounds);
		RandomUniformInt(BoundsPair<unsigned int> bounds);	//	TODO: Allowed unsigned int is a bit of a hack/inproper solution, as the return type will still remain regular int!

		int generate();

	private:
		std::mt19937 generator;
		std::uniform_int_distribution<> distribution;
	};
}