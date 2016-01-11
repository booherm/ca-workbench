#ifndef CAWORKBENCH_HPP
#define CAWORKBENCH_HPP

#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <SOIL.h>
#include <iostream>
#include "OglShaderProgram.hpp"
//#include "Simplified1DCA.hpp"
//#include "Wolfram1DCA.hpp"
#include "RandomBooleanNetwork.hpp"

const GLuint GL_WINDOW_WIDTH = 1800;
const GLuint GL_WINDOW_HEIGHT = 900;
//const unsigned int rows = 200;
//const unsigned int cols = 400;
//const unsigned int rows = 200;  // max around 200x300 for RBN
//const unsigned int cols = 300;
const unsigned int rows = 50;
const unsigned int cols = 100;
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

	// CA variables
	bool renderComplete = false;
	//Wolfram1DCA* wca;
	//Simplified1DCA* sca;
	RandomBooleanNetwork* rbn;
	static RandomBooleanNetwork* theRbn;
	
	// member functions
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
	void initGridGeometry();
	void updateRenderState();
	void initGlWindow();
	void initShaders();
	void updateCellStates();
};

#endif
