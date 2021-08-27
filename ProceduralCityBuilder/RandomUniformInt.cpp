#include "RandomUniformInt.hpp"

pcb::RandomUniformInt::RandomUniformInt(int min, int max) : distribution(min, max) {
	std::random_device device;
	generator = std::mt19937(device());
}

int pcb::RandomUniformInt::generate() {
	return distribution(generator);
}