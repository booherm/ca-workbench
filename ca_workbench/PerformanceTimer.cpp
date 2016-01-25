#include "PerformanceTimer.hpp"

void PerformanceTimer::restart() {
	startTimeNanos = getSystemTimeNanos();
	timeAtLastMark = startTimeNanos;
	started = true;
}

void PerformanceTimer::markForAverage() {
	if (!started) {
		restart();
		return;
	}

	UINT64 nowNanos = getSystemTimeNanos();
	marksForAverage.push_back(nowNanos - timeAtLastMark);
	timeAtLastMark = getSystemTimeNanos();
}

void PerformanceTimer::setId(std::string id) {
	this->id = id;
}

UINT64 PerformanceTimer::stop(bool printResult) {
	UINT64 elapsedNanos = getSystemTimeNanos() - startTimeNanos;

	if (printResult) {
		std::cout << "Performance Timer " << id << " total elapsed time = " << elapsedNanos << "; Average = ";

		unsigned int marksForAverageCount = marksForAverage.size();
		if (marksForAverageCount) {
			UINT64 totalElapsed = 0;
			for (unsigned int i = 0; i < marksForAverageCount; i++) {
				totalElapsed += marksForAverage[i];
			}

			std::cout << (totalElapsed / marksForAverageCount) << std::endl;
		}
		else
			std::cout << elapsedNanos << std::endl;
	}

	started = false;
	return elapsedNanos;
}
