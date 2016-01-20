#include "Util.hpp"

void lPad(std::string &str, const size_t num, const char paddingChar)
{
	if (num > str.size())
		str.insert(0, num - str.size(), paddingChar);
}

UINT64 getSystemTimeNanos()
{
	FILETIME ft;
	LARGE_INTEGER li;

	/* Get the amount of 100 nano seconds intervals elapsed since January 1, 1601 (UTC) and copy it
	* to a LARGE_INTEGER structure. */
	GetSystemTimeAsFileTime(&ft);
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;

	UINT64 ret = li.QuadPart;
	ret -= 116444736000000000LL; /* Convert from file time to UNIX epoch time. */

	return ret;
}

void uint64ToString(UINT64 value, std::string& result) {
	result.clear();
	result.reserve(20); // max. 20 digits possible
	UINT64 q = value;
	do {
		result += "0123456789"[q % 10];
		q /= 10;
	} while (q);
	std::reverse(result.begin(), result.end());
}

GLfloat angleBetweenVectors(glm::vec2 baseVector, glm::vec2 toVector) {
	GLfloat theta = glm::acos(glm::dot(baseVector, toVector) / (glm::length(baseVector) * glm::length(toVector)));
	if (toVector.y < baseVector.y)
		theta = glm::two_pi<GLfloat>() - theta;

	return theta;
}
