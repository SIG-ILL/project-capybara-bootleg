#include "Stopwatch.hpp"

void pcb::Stopwatch::start() {
	startTime = std::chrono::high_resolution_clock::now();
}

void pcb::Stopwatch::stop() {
	std::chrono::steady_clock::time_point endTime = std::chrono::high_resolution_clock::now();
	lastClockedDuration = endTime - startTime;
}

long long pcb::Stopwatch::getLastClockedDurationInMicroseconds() const {
	return std::chrono::duration_cast<std::chrono::microseconds>(lastClockedDuration).count();
}

long long pcb::Stopwatch::getLastClockedDurationInMilliseconds() const {
	return std::chrono::duration_cast<std::chrono::milliseconds>(lastClockedDuration).count();
}