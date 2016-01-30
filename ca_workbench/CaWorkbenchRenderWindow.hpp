#ifndef CAWORKBENCHRENDERWINDOW_HPP
#define CAWORKBENCHRENDERWINDOW_HPP

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

#include "PerformanceTimer.hpp"

const GLuint GL_WINDOW_WIDTH = 1800;
const GLuint GL_WINDOW_HEIGHT = 900;
const string SCREENSHOT_SAVE_DIRECTORY = "c:\\ca_workbench_screenshots\\";

class CaWorkbenchRenderWindow {

public:
	CaWorkbenchRenderWindow(CaWorkbenchModule* caWorkbenchModule);
	void doRenderLoop();
	~CaWorkbenchRenderWindow();

private:

	bool firstCall = true;
	PerformanceTimer updateModuleRenderDataPt;

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
	unsigned int cellTransformDataSize;
	std::vector<GLfloat> cellTransformData;
	bool pointMode = false;

	// site connections
	GLuint siteConnectionVao;
	GLuint siteConnectionModelVbo;
	GLuint siteConnectionColorVbo;
	GLuint siteConnectionTransformVbo;
	OglShaderProgram siteConnectionShaderProg;
	unsigned int siteConnectionTransformSize;
	std::vector<glm::mat4> siteConnectionTransformData;
	unsigned int siteConnectionColorsSize;
	std::vector<glm::vec4> siteConnectionColors;
	bool siteConnectionsOn = false;

	// CA workbench variables
	unsigned int screenShotId = 0;
	static CaWorkbenchRenderWindow* theCaWorkbenchRenderWindow;
	CaWorkbenchModule* module;
	static CaWorkbenchModule* theModule;
	bool paused = false;
	bool autoIterate = true;
	bool iterateOneStepFlag = false;
	unsigned int rows;
	unsigned int cols;

	// member functions
	void initGlWindow();
	void initShaders();
	void initGridGeometry();
	void initCellGeometry();
	void initSiteConnectionGeometry();
	void updateModuleRenderData();
	void updateRenderState();
	void toggleGridLines();
	void toggleSiteConnections();
	void togglePaused();
	void toggleAutoIterate();
	void iterateOneStep();
	void screenShot();
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
};

#endif
