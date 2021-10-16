#pragma once

#include <random>

namespace pcb {
	class RandomBoolean final {
	public:
		RandomBoolean();
		RandomBoolean(double trueProbability);

		bool generate();

	private:
		std::mt19937 generator;
		std::bernoulli_distribution distribution;
	};
}