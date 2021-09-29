#include <GL/glew.h>
#include <GL/freeglut.h>
#include <fstream>
#include <sstream>
#include <array>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Application.hpp"
#include "BitmapLoader.hpp"
#include "LayeredTerrainGenerator.hpp"
#include "VertexBufferObject.hpp"
#include "ShaderProgram.hpp"

#include "Logger.hpp"

std::shared_ptr<pcb::Application> pcb::Application::instance;

void pcb::Application::renderCallback() {
	instance->render();
}

void pcb::Application::reshapeCallback(int width, int height) {
	instance->reshape(width, height);
}

void pcb::Application::keyboardCallback(unsigned char key, int x, int y) {
	instance->handleKeyboard(key, x, y);
}

void pcb::Application::idleCallback() {
	instance->idleUpdate();
}

void pcb::Application::mouseMotionCallback(int x, int y) {
	instance->handleMouseMotion(x, y);
}

void pcb::Application::mouseWheelCallback(int button, int dir, int x, int y) {
	instance->handleMouseWheel(button, dir, x, y);
}

void pcb::Application::mouseCallback(int button, int state, int x, int y) {
	instance->handleMouse(button, state, x, y);
}

pcb::Application::Application() : translationX(0), translationY(0), rotationZ(0), scale(1), mouseWindowX(0), mouseWindowY(0), globalRotationX(0), globalRotationY(0),
isWarpingPointer(false), zoom(0), heightmapTexture(nullptr), generatedHeightmapTexture(nullptr), renderObjects{ nullptr, nullptr, nullptr },
terrainLayerRenderObjects(), shaderManager(), projectionMatrix(), previousGlutElapsedTime(0) {}

pcb::Application::~Application() {
	deleteResources();
}

void pcb::Application::run(std::shared_ptr<Application> instance, int argc, char* argv[]) {
	Application::instance = instance;

	initializeGLUT(argc, argv);		// Create OpenGL context before doing anything else OpenGL-related (obviously, but sometimes a necessary reminder).
	loadResources();

	glutMainLoop();
}

void pcb::Application::initializeGLUT(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1920, 1280);
	glutInitWindowPosition(320, 50);

	glutCreateWindow("Project Capybara Bootleg / Procedural City Builder");

	mouseWindowX = glutGet(GLUT_WINDOW_WIDTH) / 2;
	mouseWindowY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
	glutWarpPointer(mouseWindowX, mouseWindowY);

	glutIdleFunc(Application::idleCallback);
	glutDisplayFunc(Application::renderCallback);
	glutReshapeFunc(Application::reshapeCallback);
	glutKeyboardFunc(Application::keyboardCallback);
	glutMotionFunc(Application::mouseMotionCallback);
	glutMouseWheelFunc(Application::mouseWheelCallback);
	glutMouseFunc(Application::mouseCallback);

	glClearColor(1, 0, 1, 0);
	glEnable(GL_DEPTH_TEST);

	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		Logger logger;
		logger << "GLEW INIT FAILED WITH ERROR" << glewError << "\n";
	}
}

void pcb::Application::loadResources() {
	prepareShaders();
	generateTerrainResources();
}

void pcb::Application::generateTerrainResources() {
	int terrainSize = 512;
	LayeredTerrainGenerator terrainGenerator(terrainSize, terrainSize, 1);
	//std::unique_ptr<LayeredTerrain> terrain = terrainGenerator.generate();
	std::unique_ptr<LayeredTerrain> terrain = terrainGenerator.generateRandom();

	std::unique_ptr<HeightmapImage> heightmapImage = terrainGenerator.getHeightmap24BitImage();
	heightmapTexture = std::make_unique<Texture>(*(heightmapImage->finalImage));

	std::unique_ptr<Heightmap> generatedHeightmap = terrain->generateHeightmap();
	std::unique_ptr<Image> generatedHeightmapImage = generatedHeightmap->to24BitImage();
	generatedHeightmapTexture = std::make_unique<Texture>(*generatedHeightmapImage);

	//std::array<GLfloat, 12> quadVertices = {
	std::shared_ptr<std::vector<GLfloat>> quadVertices = std::make_shared<std::vector<GLfloat>>(std::initializer_list<GLfloat>{
		-0.25f, -0.25f, 0.0f,
		0.0f, -0.25f, 0.0f,
		0.0f, 0.0f, 0.0f,
		-0.25f, 0.0f, 0.0f
	});

	//std::array<GLfloat, 8> quadTextureCoordinates = {
	std::shared_ptr<std::vector<GLfloat>> quadTextureCoordinates = std::make_shared<std::vector<GLfloat>>(std::initializer_list<GLfloat>{
		0.0, 0.0,
		0.0, 1.0,
		1.0, 1.0,
		1.0, 0.0
	});

	std::unique_ptr<VertexPositionBufferObject> terrainVertices = std::make_unique<VertexPositionBufferObject>(terrain->getQuadsVertices(), terrain->getQuadsVertexCount());
	std::unique_ptr<VertexColorBufferObject> terrainColors = std::make_unique<VertexColorBufferObject>(terrain->getQuadsColors(), 3, terrain->getQuadsVertexCount());
	float terrainScale = 2.5f;
	std::unique_ptr<SimpleColoredObject> terrainObject = std::make_unique<SimpleColoredObject>(std::move(terrainVertices), std::move(terrainColors));
	terrainObject->setPosition(-0.2f, -0.25f, -0.5f);
	terrainObject->setScale(terrainScale, terrainScale, terrainScale);

	std::shared_ptr<VertexPositionBufferObject> heightMapImageObjectVertices = std::make_shared<VertexPositionBufferObject>(quadVertices, quadVertices->size() / 3);
	std::shared_ptr<VertexTextureCoordinateBufferObject> heightMapImageObjectTextureCoordinates = std::make_shared<VertexTextureCoordinateBufferObject>(quadTextureCoordinates, quadTextureCoordinates->size() / 2);
	float heightmapHorizontalScale = 1.0f / (static_cast<float>(glutGet(GLUT_WINDOW_WIDTH)) / static_cast<float>(glutGet(GLUT_WINDOW_HEIGHT)));
	std::unique_ptr<SimpleTexturedObject> heightmapImageObject = std::make_unique<SimpleTexturedObject>(heightMapImageObjectVertices, *heightmapTexture, heightMapImageObjectTextureCoordinates);
	heightmapImageObject->setPosition(1.0f, 1.0f, -1.0f);
	heightmapImageObject->setScale(heightmapHorizontalScale, 1.0f, 1.0f);
	std::unique_ptr<SimpleTexturedObject> generatedHeightmapObject = std::make_unique<SimpleTexturedObject>(heightMapImageObjectVertices, *generatedHeightmapTexture, heightMapImageObjectTextureCoordinates);
	generatedHeightmapObject->setPosition(1.0f, 0.74f, -1.0f);
	generatedHeightmapObject->setScale(heightmapHorizontalScale, 1.0f, 1.0f);

	renderObjects[0] = std::move(terrainObject);
	renderObjects[1] = std::move(heightmapImageObject);
	renderObjects[2] = std::move(generatedHeightmapObject);

	std::vector<std::shared_ptr<Terrain>> terrainLayers = terrain->getLayers();
	for (unsigned int i = 0; i < terrainLayers.size(); i++) {
		const Terrain& terrainLayer = *(terrainLayers.at(i));
		std::shared_ptr<VertexPositionBufferObject> terrainlayerVertices = std::make_shared<VertexPositionBufferObject>(terrainLayer.getQuadsVertices(), terrainLayer.getQuadsVertexCount());
		std::shared_ptr<VertexColorBufferObject> terrainLayerColors = std::make_shared<VertexColorBufferObject>(terrainLayer.getQuadsColors(), 3, terrainLayer.getQuadsVertexCount());
		terrainLayerRenderObjects.emplace_back(terrainlayerVertices, terrainLayerColors);
		SimpleColoredObject& object = terrainLayerRenderObjects.back();
		object.setPosition(10.0f, static_cast<float>(i + 1), -0.5f);
		object.setScale(terrainScale, terrainScale, terrainScale);
	}
}

void pcb::Application::deleteResources() {
	terrainLayerRenderObjects.clear();
}

void pcb::Application::prepareShaders() {
	std::string shaderSource = loadShaderFromFile("Shaders\\Default.vert");
	shaderManager.createVertexShader("defaultVertex", shaderSource);
	shaderSource = loadShaderFromFile("Shaders\\Default.frag");
	shaderManager.createFragmentShader("defaultFragment", shaderSource);
	shaderManager.createProgram("defaultProgram", "defaultVertex", "defaultFragment");

	shaderSource = loadShaderFromFile("Shaders\\Textured.vert");
	shaderManager.createVertexShader("texturedVertex", shaderSource);
	shaderSource = loadShaderFromFile("Shaders\\Textured.frag");
	shaderManager.createFragmentShader("texturedFragment", shaderSource);
	shaderManager.createProgram("texturedProgram", "texturedVertex", "texturedFragment");

	shaderSource = loadShaderFromFile("Shaders\\TempElevationColors.vert");
	shaderManager.createVertexShader("tempElevationColorsVertex", shaderSource);
	shaderManager.createProgram("tempElevationColorsProgram", "tempElevationColorsVertex", "defaultFragment");

	shaderSource = loadShaderFromFile("Shaders\\UI.vert");
	shaderManager.createVertexShader("userInterfaceProgramVertex", shaderSource);
	shaderManager.createProgram("userInterfaceProgram", "userInterfaceProgramVertex", "texturedFragment");

	shaderManager.useProgram("defaultProgram");
}

std::string pcb::Application::loadShaderFromFile(std::string filepath) const {
	std::ifstream fileStreamVertex(filepath);
	std::stringstream stringStreamVertex;
	stringStreamVertex << fileStreamVertex.rdbuf();
	fileStreamVertex.close();

	return stringStreamVertex.str();
}

void pcb::Application::drawTestShapes() const {
	/*for (const SimpleObject* const object : renderObjects) {
		object->render();
	}*/

	//shaderManager.useProgram("defaultProgram");
	shaderManager.useProgram("tempElevationColorsProgram");
	renderObjects[0]->render();

	shaderManager.useProgram("userInterfaceProgram");
	renderObjects[1]->render();
	renderObjects[2]->render();

	shaderManager.useProgram("defaultProgram");

	for (const pcb::SimpleColoredObject& terrainLayerObject : terrainLayerRenderObjects) {
		terrainLayerObject.render();
	}
}

void pcb::Application::render() {
	//int glutElapsedTime = glutGet(GLUT_ELAPSED_TIME);
	//int milisecondsSinceLastRenderStart = glutElapsedTime - previousGlutElapsedTime;
	//previousGlutElapsedTime = glutElapsedTime;
	//std::cout << "Time between render calls: " << milisecondsSinceLastRenderStart << ", which would be " << (1.0f / (milisecondsSinceLastRenderStart / 1000.0f)) << " FPS\n";

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 identityMatrix(1);
	glm::mat4 viewMatrix = glm::translate(identityMatrix, glm::vec3(-translationX, -translationY, static_cast<GLfloat>(zoom)));
	viewMatrix = glm::rotate(viewMatrix, 0.1f * static_cast<GLfloat>(globalRotationX), glm::vec3(1, 0, 0));
	viewMatrix = glm::rotate(viewMatrix, 0.1f * static_cast<GLfloat>(globalRotationY), glm::vec3(0, 1, 0));

	shaderManager.useProgram("defaultProgram");
	glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	shaderManager.useProgram("texturedProgram");
	glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	shaderManager.useProgram("tempElevationColorsProgram");
	glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	drawTestShapes();

	glutSwapBuffers();
}

void pcb::Application::reshape(int width, int height) {
	glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));

	projectionMatrix = glm::perspective(90.0, static_cast<double>(width) / static_cast<double>(height), 0.1, 1000.0);
}

void pcb::Application::handleKeyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'a':
		translationX -= 0.1f;
		break;
	case 'd':
		translationX += 0.1f;
		break;
	case 'w':
		translationY += 0.1f;
		break;
	case 's':
		translationY -= 0.1f;
		break;
	case 'q':
		rotationZ -= 1;
		break;
	case 'e':
		rotationZ += 1;
		break;
	case 'z':
		scale -= 0.05f;
		break;
	case 'x':
		scale += 0.05f;
		break;
	case 'g':
		deleteResources();
		generateTerrainResources();
		break;
	}
}

void pcb::Application::idleUpdate() const {
	glutPostRedisplay();
}

void pcb::Application::handleMouseMotion(int x, int y) {
	if (isWarpingPointer) {
		isWarpingPointer = false;
		return;
	}

	globalRotationX += (y - mouseWindowY);
	globalRotationY += (x - mouseWindowX);

	mouseWindowX = glutGet(GLUT_WINDOW_WIDTH) / 2;
	mouseWindowY = glutGet(GLUT_WINDOW_HEIGHT) / 2;

	isWarpingPointer = true;
	glutWarpPointer(mouseWindowX, mouseWindowY);
}

void pcb::Application::handleMouseWheel(int button, int dir, int x, int y) {
	zoom += (0.5 * dir);
}

void pcb::Application::handleMouse(int button, int state, int x, int y) {
	mouseWindowX = x;
	mouseWindowY = y;
}