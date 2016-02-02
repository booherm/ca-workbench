#include "CaWorkbenchRenderWindow.hpp"

using namespace std;

CaWorkbenchRenderWindow* CaWorkbenchRenderWindow::theCaWorkbenchRenderWindow;
CaWorkbenchModule* CaWorkbenchRenderWindow::theModule;

CaWorkbenchRenderWindow::CaWorkbenchRenderWindow(CaWorkbenchModule* caWorkbenchModule)
{
	module = caWorkbenchModule;
	theModule = module;
	theCaWorkbenchRenderWindow = this;

	rows = module->getRowCount();
	cols = module->getColumnCount();
	xInc = 1.0f / cols;
	yInc = 1.0f / rows;

	// initialize OpenGL and object geometry
	initGlWindow();
	initShaders();
	initGridGeometry();
	initCellGeometry();
	initSiteConnectionGeometry();

	updateModuleRenderDataPt.setId("RENDER_PERF_TIMER");
}

void CaWorkbenchRenderWindow::initGlWindow()
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

	// enable face culling since we're only going to render in 2d
	glCullFace(GL_FRONT);
	glEnable(GL_CULL_FACE);

	// enable color alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClearColor(0.941f, 0.941f, 0.941f, 0.0f);
}

void CaWorkbenchRenderWindow::initShaders()
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

	// site connection vertex shader
	string siteConnectionVertexShaderSource =
		"#version 330 core\n"
		"\n"
		"layout (location = 0) in vec2 position;\n"
		"layout (location = 1) in vec4 inColor;\n"
		"layout (location = 2) in mat4 instanceMatrix;\n"
		"\n"
		"out vec4 fragShaderColor;\n"
		"\n"
		"void main()\n"
		"{\n"
		"    gl_Position = instanceMatrix * vec4(position, 0.0f, 1.0f);\n"
		"    fragShaderColor = inColor;\n"
		"}\n";

	// site connection fragment shader
	string siteConnectionFragmentShaderSource =
		"#version 330 core\n"
		"\n"
		"in vec4 fragShaderColor;\n"
		"out vec4 color;\n"
		"\n"
		"void main()\n"
		"{\n"
		"    color = fragShaderColor;\n"
		"}\n";

	siteConnectionShaderProg = OglShaderProgram();
	siteConnectionShaderProg.createVertexShaderFromSourceString(siteConnectionVertexShaderSource);
	siteConnectionShaderProg.createFragmentShaderFromSourceString(siteConnectionFragmentShaderSource);
	siteConnectionShaderProg.build();
}

void CaWorkbenchRenderWindow::initGridGeometry()
{
	// initialize buffers and vertex array object
	GLuint vertTranslationVbo;
	GLuint horzTranslationVbo;
	glGenVertexArrays(1, &vertGridVao);
	glGenVertexArrays(1, &horzGridVao);
	glGenBuffers(1, &vertGridModelVbo);
	glGenBuffers(1, &vertTranslationVbo);
	glGenBuffers(1, &horzGridModelVbo);
	glGenBuffers(1, &horzTranslationVbo);

	// buffer vertical grid line model
	GLfloat verticalLineModelVertices[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glBindBuffer(GL_ARRAY_BUFFER, vertGridModelVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticalLineModelVertices), verticalLineModelVertices, GL_STATIC_DRAW);

	// buffer vertical grid line translations
	vector<GLfloat> verticalGridLineTranslations(2 * cols);
	unsigned int verticalGridLineTranslationsIndex = 0;
	for (unsigned int i = 0; i < cols; i++)
	{
		verticalGridLineTranslations[verticalGridLineTranslationsIndex++] = i * xInc;
		verticalGridLineTranslations[verticalGridLineTranslationsIndex++] = 0.0f;
	}
	glBindBuffer(GL_ARRAY_BUFFER, vertTranslationVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verticalGridLineTranslations.size(), verticalGridLineTranslations.data(), GL_STATIC_DRAW);

	// start vertex array object setup
	glBindVertexArray(vertGridVao);

	// define position attribute (model)
	glBindBuffer(GL_ARRAY_BUFFER, vertGridModelVbo);
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
	GLfloat horizontalLineModelVertices[] = { 0.0f, 0.0f, 1.0f, 0.0f };
	glBindBuffer(GL_ARRAY_BUFFER, horzGridModelVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(horizontalLineModelVertices), horizontalLineModelVertices, GL_STATIC_DRAW);

	// buffer horizontal grid line translations
	vector<GLfloat> horizontalGridLineTranslations(2 * rows);
	unsigned int horizontalGridLineTranslationsIndex = 0;
	for (unsigned int i = 0; i < rows; i++)
	{
		horizontalGridLineTranslations[horizontalGridLineTranslationsIndex++] = 0.0f;
		horizontalGridLineTranslations[horizontalGridLineTranslationsIndex++] = i * yInc;
	}
	glBindBuffer(GL_ARRAY_BUFFER, horzTranslationVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * horizontalGridLineTranslations.size(), horizontalGridLineTranslations.data(), GL_STATIC_DRAW);

	// start vertex array object setup
	glBindVertexArray(horzGridVao);

	// define position attribute (model)
	glBindBuffer(GL_ARRAY_BUFFER, horzGridModelVbo);
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

void CaWorkbenchRenderWindow::initCellGeometry() {

	// init buffers and vertex array object
	cellTransformDataSize = rows * cols * 5;   // 2 floats for translation + 3 floats for color = 5
	cellTransformData.resize(cellTransformDataSize);
	glGenVertexArrays(1, &cellStatesVao);
	glGenBuffers(1, &cellModelVbo);
	glGenBuffers(1, &cellTransformVbo);

	// buffer cell model data
	glBindBuffer(GL_ARRAY_BUFFER, cellModelVbo);
	if (pointMode) {
		// point vertices for drawing as points
		GLfloat cellModelPointVertices[] = { xInc / 2.0f, yInc / 2.0f };
		glBufferData(GL_ARRAY_BUFFER, sizeof(cellModelPointVertices), cellModelPointVertices, GL_STATIC_DRAW);
		glPointSize(GL_WINDOW_WIDTH / (GLfloat)cols);
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

	// initialize cell transform buffer
	glBindBuffer(GL_ARRAY_BUFFER, cellTransformVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * cellTransformDataSize, NULL, GL_STREAM_DRAW);

	// start vertex array object setup
	glBindVertexArray(cellStatesVao);

	// define position attribute (model)
	glBindBuffer(GL_ARRAY_BUFFER, cellModelVbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// define offset attribute (instanced)
	glBindBuffer(GL_ARRAY_BUFFER, cellTransformVbo);
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

void CaWorkbenchRenderWindow::initSiteConnectionGeometry() {

	// init buffers and vertex array object
	siteConnectionTransformSize = theModule->getMaxSiteConnectionsCount();
	siteConnectionTransformData.resize(siteConnectionTransformSize);
	siteConnectionColorsSize = siteConnectionTransformSize;
	siteConnectionColors.resize(siteConnectionColorsSize);
	glGenVertexArrays(1, &siteConnectionVao);
	glGenBuffers(1, &siteConnectionModelVbo);
	glGenBuffers(1, &siteConnectionColorVbo);
	glGenBuffers(1, &siteConnectionTransformVbo);

	// buffer site connection model
	GLfloat siteConnectionModelVertices[] = {
		0.0f, // tail x
		0.0f, // tail y
		1.0f, // head x
		0.0f  // head y
	};
	glBindBuffer(GL_ARRAY_BUFFER, siteConnectionModelVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(siteConnectionModelVertices), siteConnectionModelVertices, GL_STATIC_DRAW);

	// init site connection color buffer
	glBindBuffer(GL_ARRAY_BUFFER, siteConnectionColorVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * siteConnectionColorsSize, NULL, GL_STREAM_DRAW);

	// init site connection transform buffer
	glBindBuffer(GL_ARRAY_BUFFER, siteConnectionTransformVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * siteConnectionTransformSize, NULL, GL_STREAM_DRAW);

	// start vertex array object setup
	glBindVertexArray(siteConnectionVao);

	// define position attribute (model)
	glBindBuffer(GL_ARRAY_BUFFER, siteConnectionModelVbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// define color attribute (instanced)
	glBindBuffer(GL_ARRAY_BUFFER, siteConnectionColorVbo);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (GLvoid*)0);
	glVertexAttribDivisor(1, 1);
	glEnableVertexAttribArray(1);

	// define transform attribute (instanced)
	glBindBuffer(GL_ARRAY_BUFFER, siteConnectionTransformVbo);
	for (unsigned int i = 2; i <= 5; i++) {
		glVertexAttribPointer(i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)((i - 2) * sizeof(glm::vec4)));
		glVertexAttribDivisor(i, 1);
		glEnableVertexAttribArray(i);
	}

	// end vertex array object setup
	glBindVertexArray(0);
}

void CaWorkbenchRenderWindow::doRenderLoop()
{
	while (!glfwWindowShouldClose(glWindow)) {
		glfwPollEvents();

		// update rendering data and render state
		updateModuleRenderData();
		updateModuleRenderDataPt.markForAverage();

		updateRenderState();

		if (!paused && (autoIterate || iterateOneStepFlag)) {
			// iterate module logical state
			if (!module->getRenderComplete())
				module->iterate();
			iterateOneStepFlag = false;
		}
	}

	// cleanup
	glDeleteBuffers(1, &siteConnectionModelVbo);
	glDeleteBuffers(1, &siteConnectionColorVbo);
	glDeleteBuffers(1, &siteConnectionTransformVbo);
	glDeleteBuffers(1, &cellModelVbo);
	glDeleteBuffers(1, &cellTransformVbo);
	glDeleteBuffers(1, &horzGridModelVbo);
	glDeleteBuffers(1, &vertGridModelVbo);
	glDeleteVertexArrays(1, &siteConnectionVao);
	glDeleteVertexArrays(1, &cellStatesVao);
	glDeleteVertexArrays(1, &horzGridVao);
	glDeleteVertexArrays(1, &vertGridVao);

	// close window
	glfwTerminate();
}

void CaWorkbenchRenderWindow::updateModuleRenderData()
{
	cellTransformDataSize = 0;
	siteConnectionTransformSize = 0;
	siteConnectionColorsSize = 0;
	unsigned int siteIndex = 0;

	// process sites and site connections
	for (unsigned int r = 0; r < rows; r++) {
		unsigned int row = rows - r - 1; // logical location row (x) is top down, but the GL window row (x) is bottom up
		GLfloat transY = row * yInc;

		for (unsigned int c = 0; c < cols; c++) {

			if (module->getSiteActive(siteIndex)) {
				// translate logical cell location to world space
				GLfloat transX = c * xInc;

				// cell translation
				cellTransformData[cellTransformDataSize++] = transX;
				cellTransformData[cellTransformDataSize++] = transY;

				// cell color
				vector<float>* color = module->getSiteColor(siteIndex);
				cellTransformData[cellTransformDataSize++] = color->at(0);
				cellTransformData[cellTransformDataSize++] = color->at(1);
				cellTransformData[cellTransformDataSize++] = color->at(2);
			}

			if (siteConnectionsOn) {
				// site connection transform

				vector<SiteConnection*>* siteConnections = theModule->getSiteConnections(siteIndex);
				unsigned int siteConnectionCount = siteConnections->size();
				for (unsigned int s = 0; s < siteConnectionCount; s++) {
					SiteConnection* scp = siteConnections->at(s);

					if (scp->shouldRender) {

						// color
						vector<float>* siteConnectionColor = &scp->color;
						glm::vec4 siteConnectionColorVector(
							siteConnectionColor->at(0),
							siteConnectionColor->at(1),
							siteConnectionColor->at(2),
							siteConnectionColor->at(3)
							);
						siteConnectionColors[siteConnectionColorsSize++] = siteConnectionColorVector;

						// translate logical cell location to world space
						unsigned int tailId = scp->sourceSiteId;
						unsigned int tailCol = tailId % cols;
						unsigned int tailRow = rows - (tailId / cols) - 1; // logical location row (x) is top down, but the GL window row (x) is bottom up
						glm::vec2 tail((((tailCol * xInc) + (xInc / 2.0f)) * 2.0f) - 1.0f, (((tailRow * yInc) + (yInc / 2.0f)) * 2.0f) - 1.0f);
						unsigned int headId = scp->destinationSiteId;
						unsigned int headCol = headId % cols;
						unsigned int headRow = rows - (headId / cols) - 1; // logical location row (x) is top down, but the GL window row (x) is bottom up
						glm::vec2 head((((headCol * xInc) + (xInc / 2.0f)) * 2.0f) - 1.0f, (((headRow * yInc) + (yInc / 2.0f)) * 2.0f) - 1.0f);

						// translate to tail location
						glm::mat4 transform;
						transform = glm::translate(transform, glm::vec3(tail, 0.0f));

						// scale to distance between tail and head
						GLfloat distance = glm::distance(tail, head);
						transform = glm::scale(transform, glm::vec3(distance, distance, 1.0f));

						// rotate to point to head
						GLfloat theta = angleBetweenVectors(glm::vec2(1.0f, 0.0f), glm::vec2(head.x - tail.x, head.y - tail.y));
						transform = glm::rotate(transform, theta, glm::vec3(0.0f, 0.0f, 1.0f));

						siteConnectionTransformData[siteConnectionTransformSize++] = transform;
					}
				}
			}

			siteIndex++;
		}
	}

	// buffer cell transform data
	glBindBuffer(GL_ARRAY_BUFFER, cellTransformVbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * cellTransformDataSize, cellTransformData.data());

	if (siteConnectionsOn) {
		// buffer site connection color data
		glBindBuffer(GL_ARRAY_BUFFER, siteConnectionColorVbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4) * siteConnectionColorsSize, siteConnectionColors.data());

		// buffer site connection transform data
		glBindBuffer(GL_ARRAY_BUFFER, siteConnectionTransformVbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * siteConnectionTransformSize, siteConnectionTransformData.data());
	}
}

void CaWorkbenchRenderWindow::updateRenderState() {

	// clear current frame
	glClear(GL_COLOR_BUFFER_BIT);

	// draw cell states
	cellShaderProg.use();
	glBindVertexArray(cellStatesVao);
	if (pointMode)
		glDrawArraysInstanced(GL_POINTS, 0, 1, rows * cols);
	else {
		// 6 vertices per model, (2 translation + 3 color) = 5 buffer elements per instance
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, cellTransformDataSize / 5);
	}

	// draw grid
	if (gridLinesOn) {
		gridShaderProg.use();
		glBindVertexArray(vertGridVao);
		glDrawArraysInstanced(GL_LINES, 0, 2, cols);
		glBindVertexArray(horzGridVao);
		glDrawArraysInstanced(GL_LINES, 0, 2, rows);
	}

	// draw site connections
	if (siteConnectionsOn) {
		siteConnectionShaderProg.use();
		glBindVertexArray(siteConnectionVao);
		glDrawArraysInstanced(GL_LINES, 0, 2, siteConnectionTransformSize);
	}

	// publish frame
	glfwSwapBuffers(glWindow);
}

void CaWorkbenchRenderWindow::toggleGridLines() {
	cout << "setting grid lines " << (gridLinesOn ? "off" : "on") << endl;
	gridLinesOn = !gridLinesOn;
}

void CaWorkbenchRenderWindow::toggleSiteConnections() {
	siteConnectionsOn = !siteConnectionsOn;
}

void CaWorkbenchRenderWindow::togglePaused() {
	if (!autoIterate) {
		cout << "auto iterate is currently off, ignoring pause toggle request" << endl;
		return;
	}
	paused = !paused;
}

void CaWorkbenchRenderWindow::toggleAutoIterate() {
	cout << "setting auto iterate " << (autoIterate ? "off" : "on") << endl;
	autoIterate = !autoIterate;
}

void CaWorkbenchRenderWindow::iterateOneStep() {
	if (autoIterate) {
		cout << "auto iterate is on, ignoring iterate one step request" << endl;
		return;
	}
	iterateOneStepFlag = true;
}

void CaWorkbenchRenderWindow::screenShot() {
	string id = to_string(screenShotId++);
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

void CaWorkbenchRenderWindow::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {

	theModule->handleInputAction(action, key);

	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		case GLFW_KEY_G:
			theCaWorkbenchRenderWindow->toggleGridLines();
			break;
		case GLFW_KEY_PRINT_SCREEN:
			theCaWorkbenchRenderWindow->screenShot();
			break;
		case GLFW_KEY_PAUSE:
			theCaWorkbenchRenderWindow->togglePaused();
			break;
		case GLFW_KEY_V:
			theCaWorkbenchRenderWindow->toggleSiteConnections();
			break;
		case GLFW_KEY_W:
			theCaWorkbenchRenderWindow->toggleAutoIterate();
			break;
		case GLFW_KEY_ENTER:
			theCaWorkbenchRenderWindow->iterateOneStep();
			break;
		}
	}
}

void CaWorkbenchRenderWindow::handleInputCommand(std::string command) {
	if (command == "toggleGridLines") {
		toggleGridLines();
	}
	else if (command == "screenShot") {
		screenShot();
	}
	else if (command == "togglePaused") {
		togglePaused();
	}
	else if (command == "toggleSiteConnections") {
		toggleSiteConnections();
	}
	else if (command == "toggleAutoIterate") {
		toggleAutoIterate();
	}
	else if (command == "iterateOneStep") {
		iterateOneStep();
	}
	else if (command == "closeWindow") {
		glfwSetWindowShouldClose(glWindow, GL_TRUE);
	}
}

CaWorkbenchRenderWindow::~CaWorkbenchRenderWindow()
{
	updateModuleRenderDataPt.stop();
}
