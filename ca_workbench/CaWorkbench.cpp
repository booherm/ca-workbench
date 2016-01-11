#include "CaWorkbench.hpp"

using namespace std;

RandomBooleanNetwork* CaWorkbench::theRbn;

CaWorkbench::CaWorkbench()
{
	initGlWindow();
	initShaders();
	//wca = new Wolfram1DCA(rows, cols);
	//sca = new Simplified1DCA(rows, cols);
	rbn = new RandomBooleanNetwork(rows, cols, 2, 1, 0, false);
	theRbn = rbn;
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
		"    gl_Position = vec4(((position.x + offset.x) * 2.0) - 1.0, ((position.y + offset.y) * 2.0) - 1.0, 0.0, 1.0);\n"
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
		"    gl_Position = vec4(((position.x + offset.x) * 2.0) - 1.0, ((position.y + offset.y) * 2.0) - 1.0, 0.0, 1.0);\n"
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
}

void CaWorkbench::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
		glfwSetWindowShouldClose(window, GL_TRUE);
	else if (action == GLFW_PRESS && key == GLFW_KEY_SPACE)
		theRbn->resetCellStates();
	else if (action == GLFW_PRESS && key == GLFW_KEY_R)
		theRbn->updateInputSites();
}

void CaWorkbench::doRenderLoop()
{
	initGridGeometry();

	while (!glfwWindowShouldClose(glWindow)) {
		glfwPollEvents();

		updateCellStates();
		updateRenderState();

		if (renderComplete) {
			rbn->resetCellStates();
			/*
			//unsigned int currentRuleNumber = wca->getActiveRuleNumber();
			unsigned int currentRuleNumber = rbn->getActiveRuleNumber();
			cout << "rendering complete for rule " << currentRuleNumber << endl;

			//string filename = "c:\\2\\rule_" + to_string(currentRuleNumber) + ".bmp";
			//cout << "filename = " << filename << endl;
			
			//int save_result = SOIL_save_screenshot(
				//filename.c_str(),
				//SOIL_SAVE_TYPE_BMP,
				//0, 0, GL_WINDOW_WIDTH, GL_WINDOW_HEIGHT
			//);

			//wca->setActiveRuleNumber((currentRuleNumber + 1) % 256);
			//wca->resetCellStates();

			rbn->setActiveRuleNumber((currentRuleNumber + 1) % 4);
			rbn->resetCellStates();
			*/
			renderComplete = false;
			
		}
	}

	glDeleteBuffers(1, &vertGridVbo);
	glDeleteBuffers(1, &horzGridVbo);
	glDeleteVertexArrays(1, &vertGridVao);
	glDeleteVertexArrays(1, &horzGridVao);

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
	GLfloat verticalLineVertices[] = {0.0f, 0.0f, 0.0f, 1.0f};
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

	// draw grid
	gridShaderProg.use();
	glBindVertexArray(vertGridVao);
	glDrawArraysInstanced(GL_LINES, 0, 2, cols);
	glBindVertexArray(horzGridVao);
	glDrawArraysInstanced(GL_LINES, 0, 2, rows);

	// draw cell states
	cellShaderProg.use();
	glBindVertexArray(cellStatesVao);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, rows * cols);

	// unbinds for safety
	glBindVertexArray(0);

	// publish frame
	glfwSwapBuffers(glWindow);

	// clean up allocated vertex arrays and buffers
	glDeleteVertexArrays(1, &cellStatesVao);
	glDeleteBuffers(1, &cellTranslationVbo);
	glDeleteBuffers(1, &cellStatesVbo);
}

void CaWorkbench::updateCellStates()
{
	// iterate logical state
	//renderComplete = wca->iterate();
	//renderComplete = sca->iterate();
	renderComplete = rbn->iterate();

	// update rendering data from logical data
	//bool** cellStates = wca->getCellStates();
	//bool** cellStates = sca->getCellStates();
	//bool** cellStates = rbn->getCellStates();
	std::vector<Site>* sites = rbn->getSites();
	std::vector<Site>::iterator siteIterator = sites->begin();

	// setup cell translation and color vertex data
	int vertexIndex = 0;
	GLfloat xInc = 1.0f / cols;
	GLfloat yInc = 1.0f / rows;
	GLfloat vertexData[rows * cols * 5]; // total of (rows * cols) cells, each having a 2 coordinate translation and a 3 value color (2 + 3 = 5)
	for (unsigned int r = 0; r < rows; r++) {
		for (unsigned int c = 0; c < cols; c++) {
			unsigned int row = rows - r - 1;

			Site s = *siteIterator;
			
			// translation
			GLfloat transX;
			GLfloat transY;
			if (s.currentState) { //cellStates[r][c]) {
				// translate logical cell location to world space
				transX = c * xInc;
				transY = row * yInc;
			}
			else {
				// cell is inactive, render off screen
				transX = -1.0f;
				transY = -1.0f;
			}
			vertexData[vertexIndex++] = transX;
			vertexData[vertexIndex++] = transY;

			// color
			std::vector<float>* color = &s.color;
			vertexData[vertexIndex++] = color->at(0);
			vertexData[vertexIndex++] = color->at(1);
			vertexData[vertexIndex++] = color->at(2);

			siteIterator++;
		}
	}

	// buffer translation and color vertex data
	glGenBuffers(1, &cellTranslationVbo);
	glBindBuffer(GL_ARRAY_BUFFER, cellTranslationVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// cell quad vertices
	GLfloat cellQuadVertices[] = {
		0.0f, 0.0f,  // left triangle bottom left
		0.0f, yInc,  // left triangle top left
		xInc, yInc,  // left triangle top right
		0.0f, 0.0f,  // right triangle bottom left
		xInc, yInc,  // right triangle top right
		xInc, 0.0f   // right triangle bottom right
	};
	glGenVertexArrays(1, &cellStatesVao);
	glGenBuffers(1, &cellStatesVbo);
	glBindVertexArray(cellStatesVao);
	glBindBuffer(GL_ARRAY_BUFFER, cellStatesVbo);
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

	// unbinds for safety
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

CaWorkbench::~CaWorkbench()
{
	//delete wca;
	//delete sca;
	delete rbn;
}
