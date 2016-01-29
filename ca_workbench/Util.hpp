#ifndef UTIL_HPP
#define UTIL_HPP

#include <Windows.h>
#include <string>
#include <glm.hpp>
#include <glew.h>
#include <gtc/matrix_transform.hpp>

static double timeFrequency;
static LARGE_INTEGER startupTime;

void lPad(std::string &str, const size_t num, const char paddingChar = ' ');
void initTimeVariables();
UINT64 getSystemTimeNanos();
float getSystemTimeSeconds();
void uint64ToString(UINT64 value, std::string& result);
GLfloat angleBetweenVectors(glm::vec2 baseVector, glm::vec2 toVector);

#endif
