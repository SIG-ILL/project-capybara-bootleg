#include "Logger.hpp"

#include <iostream>

pcb::Logger& pcb::Logger::operator<<(const std::string input) {
	std::cout << input;

	return *this;
}

pcb::Logger& pcb::Logger::operator<<(const int input) {
	std::cout << input;

	return *this;
}

pcb::Logger& pcb::Logger::operator<<(const unsigned int input) {
	std::cout << input;

	return *this;
}

pcb::Logger& pcb::Logger::operator<<(const float input) {
	std::cout << input;

	return *this;
}

pcb::Logger& pcb::Logger::operator<<(const double input) {
	std::cout << input;

	return *this;
}