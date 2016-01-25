#ifndef PERFORMANCETIMER_HPP
#define PERFORMANCETIMER_HPP

#include <iostream>
#include <string>
#include <vector>
#include "Util.hpp"

class PerformanceTimer
{
public:
	// public member functions
	void markForAverage();
	void restart();
	void setId(std::string id);
	UINT64 stop(bool printResult = true);

private:
	bool started = false;
	std::string id;
	UINT64 startTimeNanos;
	UINT64 timeAtLastMark;
	std::vector<UINT64> marksForAverage;
};

#endif
