#include "RandomBoolean.hpp"

pcb::RandomBoolean::RandomBoolean() : RandomBoolean(0.5) {}

pcb::RandomBoolean::RandomBoolean(double trueProbability) : distribution(trueProbability) {
	std::random_device device;
	generator = std::mt19937(device());
}

bool pcb::RandomBoolean::generate() {
	return distribution(generator);
}