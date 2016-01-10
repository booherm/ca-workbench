#ifndef OGLSHADERPROGRAM_HPP
#define OGLSHADERPROGRAM_HPP

#include <glew.h>
#include <string>

using namespace std;

class OglShaderProgram
{

public:
	OglShaderProgram();
	void createVertexShaderFromSourceString(const string& shaderSource);
	void createFragmentShaderFromSourceString(const string& shaderSource);
	void build();
	void use();

private:
	bool built;
	bool vertexShaderSet;
	bool fragmentShaderSet;
	GLuint vertexShaderId;
	GLuint fragmentShaderId;

	GLuint buildShader(GLuint shaderType, const string& shaderSource);
	GLuint programId;
	GLuint getProgramId();

};

#endif
