#ifndef CAWORKBENCH_HPP
#define CAWORKBENCH_HPP

#define GLEW_STATIC
#include "Util.hpp"
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <SOIL.h>
#include <iostream>
#include "OglShaderProgram.hpp"
#include "RandomBooleanNetwork.hpp"

const GLuint GL_WINDOW_WIDTH = 1800;
const GLuint GL_WINDOW_HEIGHT = 900;
const unsigned int rows = 200;
const unsigned int cols = 400;
const GLuint cellStatesVertexCount = rows * cols * 6;

class CaWorkbench {

public:
	CaWorkbench();
	void doRenderLoop();
	~CaWorkbench();

private:
	// OpenGL variables
	GLFWwindow* glWindow;
	GLuint vertGridVao;
	GLuint vertGridVbo;
	GLuint horzGridVao;
	GLuint horzGridVbo;
	GLuint cellStatesVao = 0;
	GLuint cellStatesVbo = 0;
	GLuint cellTranslationVbo = 0;
	OglShaderProgram gridShaderProg;
	OglShaderProgram cellShaderProg;
	bool pointMode = false;

	// CA variables
	bool renderComplete = false;
	RandomBooleanNetwork* rbn;
	static RandomBooleanNetwork* theRbn;
	unsigned int vertexDataElements = rows * cols * 5; // 2 floats for translation + 3 floats for color = 5
	GLfloat xInc = 1.0f / cols;
	GLfloat yInc = 1.0f / rows;
	std::vector<GLfloat> vertexData;
	GLfloat cellQuadVertices[12];
	GLfloat cellPointVertices[2];

	// member functions
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
	void initGridGeometry();
	void updateRenderState();
	void initGlWindow();
	void initShaders();
	void updateCellStates();
};

#endif
