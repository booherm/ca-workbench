#ifndef UTIL_HPP
#define UTIL_HPP

#include <Windows.h>
#include <string>

UINT64 getSystemTimeNanos();
void uint64ToString(UINT64 value, std::string& result);

#endif
