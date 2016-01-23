#ifndef CAWORKBENCH_HPP
#define CAWORKBENCH_HPP

#define GLEW_STATIC
#include "Util.hpp"
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <SOIL.h>
#include <iostream>
#include "OglShaderProgram.hpp"
#include "CaWorkbenchModule.hpp"

const GLuint GL_WINDOW_WIDTH = 1800;
const GLuint GL_WINDOW_HEIGHT = 900;
const string SCREENSHOT_SAVE_DIRECTORY = "c:\\ca_workbench_screenshots\\";

class CaWorkbench {

public:
	CaWorkbench(CaWorkbenchModule* caWorkbenchModule);
	void doRenderLoop();
	~CaWorkbench();

private:
	// OpenGL variables
	GLFWwindow* glWindow;

	// grid
	GLuint vertGridVao;
	GLuint horzGridVao;
	GLuint vertGridModelVbo;
	GLuint horzGridModelVbo;
	OglShaderProgram gridShaderProg;
	bool gridLinesOn = false;

	// cells
	GLuint cellStatesVao;
	GLuint cellModelVbo;
	GLuint cellTransformVbo;
	OglShaderProgram cellShaderProg;
	GLfloat xInc;
	GLfloat yInc;
	std::vector<GLfloat> cellTransformData;
	bool pointMode = false;

	// vectors
	GLuint vectorVao;
	GLuint vectorModelVbo;
	GLuint vectorTransformVbo;
	OglShaderProgram vectorShaderProg;
	std::vector<glm::mat4> vectorTransformData;
	bool vectorsOn = false;

	// CA workbench variables
	unsigned int screenShotId = 0;
	static CaWorkbench* theCaWorkbench;
	CaWorkbenchModule* module;
	static CaWorkbenchModule* theModule;
	bool paused = false;
	unsigned int rows;
	unsigned int cols;

	// member functions
	void initGlWindow();
	void initShaders();
	void initGridGeometry();
	void initCellGeometry();
	void initVectorGeometry();
	void updateCellStates();
	void updateRenderState();
	void toggleGridLines();
	void toggleVectors();
	void togglePaused();
	void screenShot();
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
};

#endif
