#include "OglShaderProgram.hpp"

OglShaderProgram::OglShaderProgram()
{
	built = false;
	vertexShaderSet = false;
	fragmentShaderSet = false;
}

void OglShaderProgram::createVertexShaderFromSourceString(const string& shaderSource)
{
	if (vertexShaderSet)
		throw string("Exception::OglShaderProgram::createVertexShaderFromSourceString -\nvertexShader already set");

	vertexShaderId = buildShader(GL_VERTEX_SHADER, shaderSource);
	vertexShaderSet = true;
}

void OglShaderProgram::createFragmentShaderFromSourceString(const string& shaderSource)
{
	if (fragmentShaderSet)
		throw string("Exception::OglShaderProgram::createFragmentShaderFromSourceString -\nfragmentShader already set");

	fragmentShaderId = buildShader(GL_FRAGMENT_SHADER, shaderSource);
	fragmentShaderSet = true;
}

void OglShaderProgram::build()
{
	if (built)
		throw string("Exception::OglShaderProgram::build -\nshader program already built");
	if (!vertexShaderSet)
		throw string("Exception::OglShaderProgram::build -\nvertexShader not set");
	if (!fragmentShaderSet)
		throw string("Exception::OglShaderProgram::build -\nfragmentShader not set");

	programId = glCreateProgram();
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	glLinkProgram(programId);

	GLint success;
	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if(!success)
	{
		GLchar infoLog[512];
		glGetProgramInfoLog(programId, 512, nullptr, infoLog);
		throw string("Exception::OglShaderProgram::build -\n") + string(infoLog);
	}

	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);

	built = true;
}

void OglShaderProgram::use()
{
	if (!built)
		throw string("Exception::OglShaderProgram::use -\nshader program not yet built");

	glUseProgram(programId);
}

GLuint OglShaderProgram::buildShader(GLuint shaderType, const string& shaderSource)
{
	GLuint shaderId = glCreateShader(shaderType);
	const GLchar* source = shaderSource.c_str();
	glShaderSource(shaderId, 1, &source, nullptr);
	glCompileShader(shaderId);

	GLint success;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar infoLog[512];
		glGetShaderInfoLog(shaderId, 512, nullptr, infoLog);
		throw string("Exception::OglShaderProgram::buildShader -\n") + string(infoLog);
	}

	return shaderId;
}

GLuint OglShaderProgram::getProgramId()
{
	return programId;
}
