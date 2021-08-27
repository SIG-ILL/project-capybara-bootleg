#pragma once

#include <random>

namespace pcb {
	class RandomBinary final {
	public:
		RandomBinary();
		RandomBinary(double trueProbability);

		bool generate();

	private:
		std::mt19937 generator;
		std::bernoulli_distribution distribution;
	};
}