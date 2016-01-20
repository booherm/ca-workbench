#include "CaWorkbench.hpp"

using namespace std;

RandomBooleanNetwork* CaWorkbench::theRbn;
CaWorkbench* CaWorkbench::theCaWorkbench;

CaWorkbench::CaWorkbench()
{
	initGlWindow();
	initShaders();
	unsigned int connectivity = 2;
	//rbn = new RandomBooleanNetwork(rows, cols, connectivity, 84, 20, 20, false, true);
	rbn = new RandomBooleanNetwork(rows, cols, connectivity, 1, 0, 0, true, false);
	theRbn = rbn;
	theCaWorkbench = this;
	
	cellVertexData.resize(cellVertexDataElements);
	attractorVectorTransformData.resize(rows * cols * connectivity * 2);

	// cell quad vertices for drawing as triangles
	cellQuadVertices[0] = 0.0f;
	cellQuadVertices[1] = 0.0f;  // left triangle bottom left
	cellQuadVertices[2] = 0.0f;
	cellQuadVertices[3] = yInc;  // left triangle top left
	cellQuadVertices[4] = xInc;
	cellQuadVertices[5] = yInc;  // left triangle top right
	cellQuadVertices[6] = 0.0f;
	cellQuadVertices[7] = 0.0f;  // right triangle bottom left
	cellQuadVertices[8] = xInc;
	cellQuadVertices[9] = yInc;  // right triangle top right
	cellQuadVertices[10] = xInc;
	cellQuadVertices[11] = 0.0f;  // right triangle bottom right

	// point vertices for drawing as points
	cellPointVertices[0] = xInc / 2.0f;
	cellPointVertices[1] = yInc / 2.0f;

	// attractor vector vertices for drawing as lines
	attractorVectorVertices[0] = 0.0f;     // tail x
	attractorVectorVertices[1] = 0.0f;     // tail y
	attractorVectorVertices[2] = 1.0f;     // head x
	attractorVectorVertices[3] = 0.0f;     // head y
	attractorVectorVertices[4] = 0.875f;   // arrow segment 1 start x
	attractorVectorVertices[5] = 0.125f;   // arrow segment 1 start y
	attractorVectorVertices[6] = 1.0f;     // arrow segment 1 end x
	attractorVectorVertices[7] = 0.0f;     // arrow segment 1 end y
	attractorVectorVertices[8] = 0.875f;   // arrow segment 2 start x
	attractorVectorVertices[9] = -0.125f;  // arrow segment 2 start y
	attractorVectorVertices[10] = 1.0f;    // arrow segment 2 end x
	attractorVectorVertices[11] = 0.0f;    // arrow segment 2 end y
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
	//	glfwWindowHint(GLFW_SAMPLES, 4);

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

	glCullFace(GL_FRONT);
	glEnable(GL_CULL_FACE);

	if (pointMode) {
		glPointSize(GL_WINDOW_WIDTH / cols);
	}

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	initGridGeometry();
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

void CaWorkbench::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// debug, split these up into static functions owned by RBN and CaWorkbench to handle their own things
	if (action == GLFW_PRESS) {
		switch(key){
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
		}
	}
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

			std::this_thread::sleep_for(std::chrono::seconds(3));
		}
	}

	// cleanup
	glDeleteBuffers(1, &vertGridVbo);
	glDeleteBuffers(1, &horzGridVbo);
	glDeleteVertexArrays(1, &vertGridVao);
	glDeleteVertexArrays(1, &horzGridVao);

	// delete other vaos, vbos?


	// close window
	glfwTerminate();
}

void CaWorkbench::initGridGeometry()
{
	// vertical grid line translations
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
	GLuint vertTranslationVbo;
	glGenBuffers(1, &vertTranslationVbo);
	glBindBuffer(GL_ARRAY_BUFFER, vertTranslationVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * cols, &verticalGridLineTranslations[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// vertical grid line vertices
	GLfloat verticalLineVertices[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glGenVertexArrays(1, &vertGridVao);
	glGenBuffers(1, &vertGridVbo);
	glBindVertexArray(vertGridVao);
	glBindBuffer(GL_ARRAY_BUFFER, vertGridVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticalLineVertices), verticalLineVertices, GL_STATIC_DRAW);

	// define position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

	// define offset attribute (instanced)
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vertTranslationVbo);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(1, 1);
	glBindVertexArray(0);

	// horizontal grid line translations
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
	GLuint horzTranslationVbo;
	glGenBuffers(1, &horzTranslationVbo);
	glBindBuffer(GL_ARRAY_BUFFER, horzTranslationVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * rows, &horizontalGridLineTranslations[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// horizontal grid line vertices
	GLfloat horizontalLineVertices[] = { 0.0f, 0.0f, 1.0f, 0.0f };
	glGenVertexArrays(1, &horzGridVao);
	glGenBuffers(1, &horzGridVbo);
	glBindVertexArray(horzGridVao);
	glBindBuffer(GL_ARRAY_BUFFER, horzGridVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(horizontalLineVertices), horizontalLineVertices, GL_STATIC_DRAW);

	// define position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

	// define offset attribute (instanced)
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, horzTranslationVbo);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(1, 1);
	glBindVertexArray(0);
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
	attractorVectorShaderProg.use();
	glBindVertexArray(attractorVectorVao);
	glDrawArraysInstanced(GL_LINES, 0, 6, attractorVectorIndex);

	// unbinds for safety
	glBindVertexArray(0);

	// publish frame
	glfwSwapBuffers(glWindow);

	// clean up allocated vertex arrays and buffers
	glDeleteVertexArrays(1, &cellStatesVao);
	glDeleteVertexArrays(1, &attractorVectorVao);
	glDeleteBuffers(1, &cellTranslationVbo);
	glDeleteBuffers(1, &cellStatesVbo);
	glDeleteBuffers(1, &attractorVectorTransformVbo);
	glDeleteBuffers(1, &attractorVectorModelVbo);
}

void CaWorkbench::updateCellStates()
{
	// iterate logical state
	//std::string sectionTimeString;
	//UINT64 sectionTimeStart = getSystemTimeNanos();
	renderComplete = rbn->iterate();
	//uint64ToString(getSystemTimeNanos() - sectionTimeStart, sectionTimeString);
	//cout << "RBN logical state updated in " << sectionTimeString << "ns" << endl;

	//std::string sectionTimeString2;
	//UINT64 sectionTimeStart2 = getSystemTimeNanos();

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
			if (s->freshActivation) {
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
					glm::vec2 outputSitePosition((((outputSiteCol * xInc) + (xInc / 2.0f)) * 2.0f) - 1.0f, (((outputSiteRow * yInc) + (yInc / 2.0f)) * 2.0f) - 1.0f);
					GLfloat distance = glm::distance(thisSitePosition, outputSitePosition);
					transform = glm::scale(transform, glm::vec3(distance, distance, 1.0f));

					// rotate to point to output site
					GLfloat theta = angleBetweenVectors(glm::vec2(1.0f, 0.0f), glm::vec2(outputSitePosition.x - thisSitePosition.x, outputSitePosition.y - thisSitePosition.y));
					transform = glm::rotate(transform, theta, glm::vec3(0.0f, 0.0f, 1.0f));

					attractorVectorTransformData[attractorVectorIndex++] = transform;
				}
			}

			siteIterator++;
		}
	}

	// buffer cell translation and color vertex data
	glGenBuffers(1, &cellTranslationVbo);
	glBindBuffer(GL_ARRAY_BUFFER, cellTranslationVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * cellVertexDataElements, cellVertexData.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &cellStatesVao);
	glGenBuffers(1, &cellStatesVbo);
	glBindVertexArray(cellStatesVao);
	glBindBuffer(GL_ARRAY_BUFFER, cellStatesVbo);
	if(pointMode)
		glBufferData(GL_ARRAY_BUFFER, sizeof(cellPointVertices), cellPointVertices, GL_STATIC_DRAW);
	else
		glBufferData(GL_ARRAY_BUFFER, sizeof(cellQuadVertices), cellQuadVertices, GL_STATIC_DRAW);

	// define position attribute
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// define offset attribute (instanced)
	glBindBuffer(GL_ARRAY_BUFFER, cellTranslationVbo);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribDivisor(1, 1);

	// define color attribute (instanced)
	glBindBuffer(GL_ARRAY_BUFFER, cellTranslationVbo);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribDivisor(2, 1);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// attractor vector transform data
	glGenBuffers(1, &attractorVectorTransformVbo);
	glBindBuffer(GL_ARRAY_BUFFER, attractorVectorTransformVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * attractorVectorIndex, attractorVectorTransformData.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &attractorVectorVao);
	glGenBuffers(1, &attractorVectorModelVbo);
	glBindVertexArray(attractorVectorVao);
	glBindBuffer(GL_ARRAY_BUFFER, attractorVectorModelVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(attractorVectorVertices), attractorVectorVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, attractorVectorTransformVbo);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribDivisor(1, 1);

	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(glm::vec4)));
	glEnableVertexAttribArray(2);
	glVertexAttribDivisor(2, 1);

	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(2 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(3);
	glVertexAttribDivisor(3, 1);

	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(3 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(4);
	glVertexAttribDivisor(4, 1);

	// unbinds for safety
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//uint64ToString(getSystemTimeNanos() - sectionTimeStart2, sectionTimeString2);
	//cout << "render data state updated in " << sectionTimeString2 << "ns" << endl;
}

void CaWorkbench::toggleGridLines() {
	cout << "setting grid lines " << (gridLinesOn ? "off" : "on") << endl;
	gridLinesOn = !gridLinesOn;
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

	if(saveResult != 1)
		cout << "unkown error saving screenshot" << endl;
}

void CaWorkbench::togglePaused() {
	paused = !paused;
}

CaWorkbench::~CaWorkbench()
{
	delete rbn;
}
