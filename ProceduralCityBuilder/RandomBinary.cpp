#include "RandomBinary.hpp"

pcb::RandomBinary::RandomBinary() : RandomBinary(0.5) {}

pcb::RandomBinary::RandomBinary(double trueProbability) : distribution(trueProbability) {
	std::random_device device;
	generator = std::mt19937(device());
}

bool pcb::RandomBinary::generate() {
	return distribution(generator);
}