#include "RandomUniformInt.hpp"

pcb::RandomUniformInt::RandomUniformInt(int min, int max) : distribution(min, max) {
	std::random_device device;
	generator = std::mt19937(device());
}

pcb::RandomUniformInt::RandomUniformInt(BoundsPair<int> bounds) : RandomUniformInt(bounds.lower, bounds.upper) {}

pcb::RandomUniformInt::RandomUniformInt(BoundsPair<unsigned int> bounds) : RandomUniformInt(bounds.lower, bounds.upper) {}

int pcb::RandomUniformInt::generate() {
	return distribution(generator);
}