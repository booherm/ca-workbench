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
#include "RandomBooleanNetwork.hpp"

#include <thread>
#include <chrono>

const GLuint GL_WINDOW_WIDTH = 1800;
const GLuint GL_WINDOW_HEIGHT = 900;
const unsigned int rows = 100;
const unsigned int cols = 200;
//const unsigned int rows = 800;
//const unsigned int cols = 1600;
const string SCREENSHOT_SAVE_DIRECTORY = "c:\\ca_workbench_screenshots\\";

class CaWorkbench {

public:
	CaWorkbench();
	void doRenderLoop();
	~CaWorkbench();

private:
	// OpenGL variables
	GLFWwindow* glWindow;

	// grid
	GLuint vertGridVao;
	GLuint horzGridVao;
	GLuint vertGridVbo;
	GLuint horzGridVbo;
	OglShaderProgram gridShaderProg;
	bool gridLinesOn = false;

	// cells
	GLuint cellStatesVao;
	GLuint cellModelVbo;
	GLuint cellTranslationVbo;
	OglShaderProgram cellShaderProg;
	GLfloat xInc = 1.0f / cols;
	GLfloat yInc = 1.0f / rows;
	std::vector<GLfloat> cellVertexData;
	bool pointMode = false;

	// attractor vectors
	GLuint attractorVectorVao;
	GLuint attractorVectorModelVbo;
	GLuint attractorVectorTransformVbo;
	OglShaderProgram attractorVectorShaderProg;
	std::vector<glm::mat4> attractorVectorTransformData;
	unsigned int attractorVectorIndex;
	bool attractorVectorsOn = false;

	// CA workbench variables
	unsigned int screenShotId = 0;
	bool renderComplete = false;
	static CaWorkbench* theCaWorkbench;
	RandomBooleanNetwork* rbn;
	static RandomBooleanNetwork* theRbn;
	bool paused = false;

	// member functions
	void initGlWindow();
	void initShaders();
	void initGridGeometry();
	void initCellGeometry();
	void initAttractorVectorGeometry();
	void updateCellStates();
	void updateRenderState();
	void toggleGridLines();
	void toggleAttractorVectors();
	void togglePaused();
	void screenShot();
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
};

#endif
