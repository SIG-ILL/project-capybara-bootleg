#include "RandomUniformReal.hpp"

pcb::RandomUniformReal::RandomUniformReal(double min, double max) : distribution(min, max) {
	std::random_device device;
	generator = std::mt19937(device());
}

double pcb::RandomUniformReal::generate() {
	return distribution(generator);
}