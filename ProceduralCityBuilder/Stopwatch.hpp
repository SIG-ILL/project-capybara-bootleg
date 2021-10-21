#pragma once

#include <chrono>
#include <string>

#include "Logger.hpp"

namespace pcb {
	class Stopwatch final {
	public:
		void start();
		void stop();
		long long getLastClockedDurationInMicroseconds() const;
		long long getLastClockedDurationInMilliseconds() const;

	private:
		std::chrono::steady_clock::time_point startTime;
		std::chrono::steady_clock::duration lastClockedDuration;
		Logger logger;
	};
}