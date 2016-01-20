#ifndef UTIL_HPP
#define UTIL_HPP

#include <Windows.h>
#include <string>
#include <glm.hpp>
#include <glew.h>
#include <gtc/matrix_transform.hpp>

void lPad(std::string &str, const size_t num, const char paddingChar = ' ');
UINT64 getSystemTimeNanos();
void uint64ToString(UINT64 value, std::string& result);
GLfloat angleBetweenVectors(glm::vec2 baseVector, glm::vec2 toVector);

#endif
