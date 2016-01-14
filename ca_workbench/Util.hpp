#ifndef UTIL_HPP
#define UTIL_HPP

#include <Windows.h>
#include <string>

void lPad(std::string &str, const size_t num, const char paddingChar = ' ');
UINT64 getSystemTimeNanos();
void uint64ToString(UINT64 value, std::string& result);

#endif
