#include "CaWorkbench.hpp"

using namespace std;

CaWorkbench* CaWorkbench::theCaWorkbench;
RandomBooleanNetwork* CaWorkbench::theRbn;

CaWorkbench::CaWorkbench()
{
	// initialize OpenGL and object geometry
	initGlWindow();
	initShaders();
	initGridGeometry();
	initCellGeometry();
	initAttractorVectorGeometry();
	theCaWorkbench = this;

	// workbench unit - debug, generalize
	unsigned int connectivity = 3;
	rbn = new RandomBooleanNetwork(rows, cols, connectivity, 5, 5, 5, false, true);
	theRbn = rbn;
	
	// do something with these
	cellVertexData.resize(rows * cols * 5); // 2 floats for translation + 3 floats for color = 5
	attractorVectorTransformData.resize(rows * cols * connectivity * 2);
}

void CaWorkbench::initGlWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return;
	}

	cout << "CA Workbench v0.1" << endl;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	glWindow = glfwCreateWindow(GL_WINDOW_WIDTH, GL_WINDOW_HEIGHT, "CA Workbench", nullptr, nullptr);
	if (glWindow == nullptr) {
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(glWindow);
	glfwSetKeyCallback(glWindow, keyCallback);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		cout << "Failed to initialize GLEW" << endl;
		glfwTerminate();
		return;
	}

	glViewport(0, 0, GL_WINDOW_WIDTH, GL_WINDOW_HEIGHT);

	// Ensure we can capture the escape key being pressed below
	//glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// enable face culling since we're only going to render in 2d
	glCullFace(GL_FRONT);
	glEnable(GL_CULL_FACE);

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

void CaWorkbench::initShaders()
{
	// grid vertex shader
	string gridVertexShaderSource =
		"#version 330 core\n"
		"\n"
		"layout (location = 0) in vec2 position;\n"
		"layout (location = 1) in vec2 offset;\n"
		"\n"
		"void main()\n"
		"{\n"
		"    gl_Position = vec4(((position.x + offset.x) * 2.0f) - 1.0f, ((position.y + offset.y) * 2.0f) - 1.0f, 0.0f, 1.0f);\n"
		"}\n";

	// grid fragment shader
	string gridFragmentShaderSource =
		"#version 330 core\n"
		"\n"
		"out vec4 color;\n"
		"\n"
		"void main()\n"
		"{\n"
		"    color = vec4(0.75f, 0.75f, 0.75f, 1.0f);\n"
		"}\n";

	gridShaderProg = OglShaderProgram();
	gridShaderProg.createVertexShaderFromSourceString(gridVertexShaderSource);
	gridShaderProg.createFragmentShaderFromSourceString(gridFragmentShaderSource);
	gridShaderProg.build();

	// cell vertex shader
	string cellVertexShaderSource =
		"#version 330 core\n"
		"\n"
		"layout (location = 0) in vec2 position;\n"
		"layout (location = 1) in vec2 offset;\n"
		"layout (location = 2) in vec3 inColor;\n"
		"\n"
		"out vec3 fragShaderColor;\n"
		"\n"
		"void main()\n"
		"{\n"
		"    gl_Position = vec4(((position.x + offset.x) * 2.0f) - 1.0f, ((position.y + offset.y) * 2.0f) - 1.0f, 0.0f, 1.0f);\n"
		"    fragShaderColor = inColor;\n"
		"}\n";

	// cell fragment shader
	string cellFragmentShaderSource =
		"#version 330 core\n"
		"\n"
		"in vec3 fragShaderColor;\n"
		"out vec4 color;\n"
		"\n"
		"void main()\n"
		"{\n"
		"    color = vec4(fragShaderColor, 1.0f);\n"
		"}\n\0";

	cellShaderProg = OglShaderProgram();
	cellShaderProg.createVertexShaderFromSourceString(cellVertexShaderSource);
	cellShaderProg.createFragmentShaderFromSourceString(cellFragmentShaderSource);
	cellShaderProg.build();

	// attractor vector vertex shader
	string attractorVectorVertexShaderSource =
		"#version 330 core\n"
		"\n"
		"layout (location = 0) in vec2 position;\n"
		"layout (location = 1) in mat4 instanceMatrix;\n"
		"\n"
		"void main()\n"
		"{\n"
		"    gl_Position = instanceMatrix * vec4(position, 0.0f, 1.0f);\n"
		"}\n";

	// attractor vector fragment shader
	string attractorVectorFragmentShaderSource =
		"#version 330 core\n"
		"\n"
		"out vec4 color;\n"
		"\n"
		"void main()\n"
		"{\n"
		"    color = vec4(0.0f, 1.0f, 0.0f, 1.0f);\n"
		"}\n";

	attractorVectorShaderProg = OglShaderProgram();
	attractorVectorShaderProg.createVertexShaderFromSourceString(attractorVectorVertexShaderSource);
	attractorVectorShaderProg.createFragmentShaderFromSourceString(attractorVectorFragmentShaderSource);
	attractorVectorShaderProg.build();
}

void CaWorkbench::initGridGeometry()
{
	// initialize buffers and vertex array object
	GLuint vertTranslationVbo;
	GLuint horzTranslationVbo;
	glGenVertexArrays(1, &vertGridVao);
	glGenVertexArrays(1, &horzGridVao);
	glGenBuffers(1, &vertGridVbo);
	glGenBuffers(1, &vertTranslationVbo);
	glGenBuffers(1, &horzGridVbo);
	glGenBuffers(1, &horzTranslationVbo);

	// buffer vertical grid line model
	GLfloat verticalLineVertices[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glBindBuffer(GL_ARRAY_BUFFER, vertGridVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticalLineVertices), verticalLineVertices, GL_STATIC_DRAW);

	// buffer vertical grid line translations
	int vertTranslationIndex = 0;
	glm::vec2 verticalGridLineTranslations[cols];
	GLfloat fInc = 1.0f / cols;
	for (unsigned int i = 0; i < cols; i++)
	{
		glm::vec2 translation;
		translation.x = i * fInc;
		translation.y = 0.0f;
		verticalGridLineTranslations[vertTranslationIndex++] = translation;
	}
	glBindBuffer(GL_ARRAY_BUFFER, vertTranslationVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * cols, &verticalGridLineTranslations[0], GL_STATIC_DRAW);

	// start vertex array object setup
	glBindVertexArray(vertGridVao);

	// define position attribute (model)
	glBindBuffer(GL_ARRAY_BUFFER, vertGridVbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// define offset attribute (instanced)
	glBindBuffer(GL_ARRAY_BUFFER, vertTranslationVbo);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribDivisor(1, 1);
	glEnableVertexAttribArray(1);

	// end vertex array object setup
	glBindVertexArray(0);

	// buffer horizontal grid line vertices
	GLfloat horizontalLineVertices[] = { 0.0f, 0.0f, 1.0f, 0.0f };
	glBindBuffer(GL_ARRAY_BUFFER, horzGridVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(horizontalLineVertices), horizontalLineVertices, GL_STATIC_DRAW);

	// buffer horizontal grid line translations
	int horzTranslationIndex = 0;
	glm::vec2 horizontalGridLineTranslations[rows];
	fInc = 1.0f / rows;
	for (unsigned int i = 0; i < rows; i++)
	{
		glm::vec2 translation;
		translation.x = 0.0f;
		translation.y = i * fInc;
		horizontalGridLineTranslations[horzTranslationIndex++] = translation;
	}
	glBindBuffer(GL_ARRAY_BUFFER, horzTranslationVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * rows, &horizontalGridLineTranslations[0], GL_STATIC_DRAW);

	// start vertex array object setup
	glBindVertexArray(horzGridVao);

	// define position attribute (model)
	glBindBuffer(GL_ARRAY_BUFFER, horzGridVbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// define offset attribute (instanced)
	glBindBuffer(GL_ARRAY_BUFFER, horzTranslationVbo);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribDivisor(1, 1);
	glEnableVertexAttribArray(1);

	// end vertex array object setup
	glBindVertexArray(0);
}

void CaWorkbench::initCellGeometry() {

	// init buffers and vertex array object
	glGenVertexArrays(1, &cellStatesVao);
	glGenBuffers(1, &cellModelVbo);
	glGenBuffers(1, &cellTranslationVbo);

	// buffer cell model data
	glBindBuffer(GL_ARRAY_BUFFER, cellModelVbo);
	if (pointMode) {
		// point vertices for drawing as points
		GLfloat cellModelPointVertices[] = {xInc / 2.0f, yInc / 2.0f};
		glBufferData(GL_ARRAY_BUFFER, sizeof(cellModelPointVertices), cellModelPointVertices, GL_STATIC_DRAW);
		glPointSize(GL_WINDOW_WIDTH / cols);
	}
	else {
		// cell quad vertices for drawing as triangles
		GLfloat cellModelQuadVertices[] = {
			0.0f, 0.0f,  // left triangle bottom left
			0.0f, yInc,  // left triangle top left
			xInc, yInc,  // left triangle top right
			0.0f, 0.0f,  // right triangle bottom left
			xInc, yInc,  // right triangle top right
			xInc, 0.0f   // right triangle bottom right
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(cellModelQuadVertices), cellModelQuadVertices, GL_STATIC_DRAW);
	}

	// start vertex array object setup
	glBindVertexArray(cellStatesVao);

	// define position attribute (model)
	glBindBuffer(GL_ARRAY_BUFFER, cellModelVbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// define offset attribute (instanced)
	glBindBuffer(GL_ARRAY_BUFFER, cellTranslationVbo);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribDivisor(1, 1);
	glEnableVertexAttribArray(1);

	// define color attribute (instanced)
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glVertexAttribDivisor(2, 1);
	glEnableVertexAttribArray(2);

	// end vertex array object setup
	glBindVertexArray(0);
}

void CaWorkbench::initAttractorVectorGeometry() {

	// init buffers and vertex array object
	glGenVertexArrays(1, &attractorVectorVao);
	glGenBuffers(1, &attractorVectorModelVbo);
	glGenBuffers(1, &attractorVectorTransformVbo);

	// buffer attractor vector model
	GLfloat attractorVectorVertices[] = {
		0.0f, // tail x
		0.0f, // tail y
		1.0f, // head x
		0.0f  // head y
	};
	glBindBuffer(GL_ARRAY_BUFFER, attractorVectorModelVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(attractorVectorVertices), attractorVectorVertices, GL_STATIC_DRAW);

	// start vertex array object setup
	glBindVertexArray(attractorVectorVao);

	// define position attribute (model)
	glBindBuffer(GL_ARRAY_BUFFER, attractorVectorModelVbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// define attractor transform attribute (instanced)
	glBindBuffer(GL_ARRAY_BUFFER, attractorVectorTransformVbo);
	for (unsigned int i = 1; i <= 4; i++) {   // don't really understand binding the matrix 4 times... need to figure it out
		glVertexAttribPointer(i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)((i - 1) * sizeof(glm::vec4)));
		glVertexAttribDivisor(i, 1);
		glEnableVertexAttribArray(i);
	}
	
	// end vertex array object setup
	glBindVertexArray(0);
}

void CaWorkbench::doRenderLoop()
{
	while (!glfwWindowShouldClose(glWindow)) {
		glfwPollEvents();

		if(!paused){
			updateCellStates();   // consistent time in regards to point vs. quad, slows as grid size increases
			updateRenderState();  // pretty consistent 1ms regardless of point vs. quad, size of grid

			if (renderComplete) {
				rbn->updateInputSites();
				renderComplete = false;
			}

		//	std::this_thread::sleep_for(std::chrono::seconds(3));
		}
	}

	// cleanup
	glDeleteBuffers(1, &cellModelVbo);
	glDeleteBuffers(1, &cellTranslationVbo);
	glDeleteBuffers(1, &attractorVectorModelVbo);
	glDeleteBuffers(1, &attractorVectorTransformVbo);
	glDeleteBuffers(1, &vertGridVbo);
	glDeleteBuffers(1, &horzGridVbo);
	glDeleteVertexArrays(1, &cellStatesVao);
	glDeleteVertexArrays(1, &attractorVectorVao);
	glDeleteVertexArrays(1, &vertGridVao);
	glDeleteVertexArrays(1, &horzGridVao);

	// close window
	glfwTerminate();
}

void CaWorkbench::updateCellStates()
{
	// iterate logical state
	renderComplete = rbn->iterate();

	// update rendering data from logical data
	std::vector<Site>* sites = rbn->getSites();
	std::vector<Site>::iterator siteIterator = sites->begin();

	// setup cell translation, color, and attractor vector transform data
	unsigned int cellVertexIndex = 0;
	attractorVectorIndex = 0;
	for (unsigned int r = 0; r < rows; r++) {
		for (unsigned int c = 0; c < cols; c++) {
			unsigned int row = rows - r - 1; // logical location row (x) is top down, but the GL window row (x) is bottom up

			Site* s = &*siteIterator;

			// cell translation
			GLfloat transX;
			GLfloat transY;
			if (s->currentState) {
				// translate logical cell location to world space
				transX = c * xInc;
				transY = row * yInc;
			}
			else {
				// cell is inactive, render off screen
				transX = -1.0f;
				transY = -1.0f;
			}
			cellVertexData[cellVertexIndex++] = transX;
			cellVertexData[cellVertexIndex++] = transY;

			// cell color
			std::vector<float>* color = &s->color;
			cellVertexData[cellVertexIndex++] = color->at(0);
			cellVertexData[cellVertexIndex++] = color->at(1);
			cellVertexData[cellVertexIndex++] = color->at(2);

			// attractor vectors
			if (attractorVectorsOn && s->freshActivation) {
				glm::vec2 thisSitePosition(((transX + (xInc / 2.0f)) * 2.0f) - 1.0f, ((transY + (yInc / 2.0f)) * 2.0f) - 1.0f);
				unsigned int outputSiteCount = s->outputSiteIds.size();
				for (unsigned int outputSiteIndex = 0; outputSiteIndex < outputSiteCount; outputSiteIndex++) {

					// translate to this site location
					glm::mat4 transform;
					transform = glm::translate(transform, glm::vec3(thisSitePosition, 0.0f));

					// scale to distance between this site and output site
					unsigned int outputSiteId = s->outputSiteIds[outputSiteIndex];
					unsigned int outputSiteRow = rows - (outputSiteId / cols) - 1;
					unsigned int outputSiteCol = outputSiteId % cols;
					glm::vec2 outputSitePosition((((outputSiteCol * xInc) + (xInc / 2.0f)) * 2.0f) - 1.0f,
						(((outputSiteRow * yInc) + (yInc / 2.0f)) * 2.0f) - 1.0f);
					GLfloat distance = glm::distance(thisSitePosition, outputSitePosition);
					transform = glm::scale(transform, glm::vec3(distance, distance, 1.0f));

					// rotate to point to output site
					GLfloat theta = angleBetweenVectors(glm::vec2(1.0f, 0.0f), glm::vec2(outputSitePosition.x - thisSitePosition.x,
						outputSitePosition.y - thisSitePosition.y));
					transform = glm::rotate(transform, theta, glm::vec3(0.0f, 0.0f, 1.0f));

					attractorVectorTransformData[attractorVectorIndex++] = transform;
				}
			}

			siteIterator++;
		}
	}

	// buffer cell vertex translation and color data
	glBindBuffer(GL_ARRAY_BUFFER, cellTranslationVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * cellVertexData.size(), cellVertexData.data(), GL_STATIC_DRAW);

	// buffer attractor vector transform data
	if (attractorVectorsOn) {
		glBindBuffer(GL_ARRAY_BUFFER, attractorVectorTransformVbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * attractorVectorIndex, attractorVectorTransformData.data(), GL_STATIC_DRAW);
	}
}

void CaWorkbench::updateRenderState()
{
	// clear current frame
	glClear(GL_COLOR_BUFFER_BIT);

	// draw cell states
	cellShaderProg.use();
	glBindVertexArray(cellStatesVao);
	if (pointMode)
		glDrawArraysInstanced(GL_POINTS, 0, 1, rows * cols);
	else
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, rows * cols);

	// draw grid
	if(gridLinesOn){
		gridShaderProg.use();
		glBindVertexArray(vertGridVao);
		glDrawArraysInstanced(GL_LINES, 0, 2, cols);
		glBindVertexArray(horzGridVao);
		glDrawArraysInstanced(GL_LINES, 0, 2, rows);
	}

	// draw attractor vectors
	if(attractorVectorsOn){
		attractorVectorShaderProg.use();
		glBindVertexArray(attractorVectorVao);
		glDrawArraysInstanced(GL_LINES, 0, 2, attractorVectorIndex);
	}

	// publish frame
	glfwSwapBuffers(glWindow);
}

void CaWorkbench::toggleGridLines() {
	cout << "setting grid lines " << (gridLinesOn ? "off" : "on") << endl;
	gridLinesOn = !gridLinesOn;
}

void CaWorkbench::toggleAttractorVectors() {
	attractorVectorsOn = !attractorVectorsOn;
}

void CaWorkbench::togglePaused() {
	paused = !paused;
}

void CaWorkbench::screenShot() {

	string id = std::to_string(screenShotId++);
	lPad(id, 5, '0');
	string filename = SCREENSHOT_SAVE_DIRECTORY + id + ".bmp";

	cout << "Saving screenshot to " << filename << endl;

	int saveResult = SOIL_save_screenshot(
		filename.c_str(),
		SOIL_SAVE_TYPE_BMP,
		0, 0, GL_WINDOW_WIDTH, GL_WINDOW_HEIGHT
		);

	if (saveResult != 1)
		cout << "unkown error saving screenshot" << endl;
}

void CaWorkbench::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	// debug, split these up into static functions owned by RBN and CaWorkbench to handle their own things
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		case GLFW_KEY_SPACE:
			theRbn->resetCellStates();
			break;
		case GLFW_KEY_R:
			theRbn->updateInputSites();
			break;
		case GLFW_KEY_F:
			theRbn->feedForward();
			break;
		case GLFW_KEY_T:
			theRbn->toggleAutoFeedForward();
			break;
		case GLFW_KEY_A:
			theRbn->toggleAutoNewInput();
			break;
		case GLFW_KEY_G:
			theCaWorkbench->toggleGridLines();
			break;
		case GLFW_KEY_UP:
			if (theRbn->setConnectivity(theRbn->getConnectivity() + 1)) {
				theCaWorkbench->attractorVectorTransformData.resize(rows * cols * theRbn->getConnectivity() * 2);
			}
			break;
		case GLFW_KEY_DOWN:
			if (theRbn->setConnectivity(theRbn->getConnectivity() - 1)) {
				theCaWorkbench->attractorVectorTransformData.resize(rows * cols * theRbn->getConnectivity() * 2);
			}
			break;
		case GLFW_KEY_LEFT:
			theRbn->setNeighborhoodConnections(false);
			break;
		case GLFW_KEY_RIGHT:
			theRbn->setNeighborhoodConnections(true);
			break;
		case GLFW_KEY_KP_1:
			theRbn->decrementExternalInputRows();
			break;
		case GLFW_KEY_KP_7:
			theRbn->incrementExternalInputRows();
			break;
		case GLFW_KEY_KP_2:
			theRbn->decrementFeedbackInputRows();
			break;
		case GLFW_KEY_KP_8:
			theRbn->incrementFeedbackInputRows();
			break;
		case GLFW_KEY_KP_3:
			theRbn->decrementExternalOutputRows();
			break;
		case GLFW_KEY_KP_9:
			theRbn->incrementExternalOutputRows();
			break;
		case GLFW_KEY_INSERT:
			theRbn->shiftInputData(0);
			break;
		case GLFW_KEY_PRINT_SCREEN:
			theCaWorkbench->screenShot();
			break;
		case GLFW_KEY_PAUSE:
			theCaWorkbench->togglePaused();
			break;
		case GLFW_KEY_V:
			theCaWorkbench->toggleAttractorVectors();
			break;
		case GLFW_KEY_I:
			theRbn->toggleFadeInactiveSites();
			break;
		}
	}
}

CaWorkbench::~CaWorkbench()
{
	delete rbn;
}
