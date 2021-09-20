#pragma once

#include <string>

namespace pcb {
	class Logger final {
	public:
		Logger& operator<<(const std::string input);
		Logger& operator<<(const int input);
		Logger& operator<<(const unsigned int input);
		Logger& operator<<(const float input);
		Logger& operator<<(const double input);
	};
}