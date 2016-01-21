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
const unsigned int rows = 300;
const unsigned int cols = 600;
//const unsigned int rows = 800;
//const unsigned int cols = 1600;
const GLuint cellStatesVertexCount = rows * cols * 6;
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
	GLuint vertGridVbo;
	GLuint horzGridVao;
	GLuint horzGridVbo;
	OglShaderProgram gridShaderProg;
	bool gridLinesOn = false;

	// cells
	GLuint cellStatesVao = 0;
	GLuint cellStatesVbo = 0;
	GLuint cellTranslationVbo = 0;
	unsigned int cellVertexDataElements = rows * cols * 5; // 2 floats for translation + 3 floats for color = 5
	GLfloat xInc = 1.0f / cols;
	GLfloat yInc = 1.0f / rows;
	std::vector<GLfloat> cellVertexData;
	OglShaderProgram cellShaderProg;
	bool pointMode = false;
	GLfloat cellQuadVertices[12];
	GLfloat cellPointVertices[2];

	// attractor vectors
	OglShaderProgram attractorVectorShaderProg;
	GLfloat attractorVectorVertices[4];
	std::vector<glm::mat4> attractorVectorTransformData;
	GLuint attractorVectorVao = 0;
	GLuint attractorVectorModelVbo = 0;
	GLuint attractorVectorTransformVbo = 0;
	unsigned int attractorVectorIndex;
	bool attractorVectorsOn = false;

	// CA workbench variables
	unsigned int screenShotId = 0;
	bool renderComplete = false;
	RandomBooleanNetwork* rbn;
	static RandomBooleanNetwork* theRbn;
	static CaWorkbench* theCaWorkbench;
	bool paused = false;

	// member functions
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
	void initGridGeometry();
	void updateRenderState();
	void initGlWindow();
	void initShaders();
	void updateCellStates();
	void toggleGridLines();
	void screenShot();
	void togglePaused();
	void toggleAttractorVectors();
};

#endif
