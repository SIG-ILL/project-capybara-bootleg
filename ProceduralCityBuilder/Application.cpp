#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>

#include "Application.hpp"
#include "BitmapLoader.hpp"
#include "HeightmapGenerator.hpp"
#include "LayeredTerrainGenerator.hpp"
#include "GLBufferObject.hpp"
#include "ShaderProgram.hpp"

pcb::Application* pcb::Application::instance;

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

pcb::Application::Application() : translationX(0), translationY(0), rotationZ(0), scale(1), mouseWindowX(0), mouseWindowY(0), globalRotationX(0), globalRotationY(0),
isWarpingPointer(false), zoom(0), heightmapTexture(nullptr), generatedHeightmapTexture(nullptr), renderObjects{ nullptr, nullptr, nullptr },
renderObjectsDataPointers{ nullptr, nullptr, nullptr }, terrainLayers(), terrainLayerRenderObjects(), vbos(), previousGlutElapsedTime(0) {}

pcb::Application::~Application() {
	delete heightmapTexture;
	delete generatedHeightmapTexture;

	for (SimpleObject* object : renderObjects) {
		delete object;
	}

	for (GLfloat* renderObjectDataPointer : renderObjectsDataPointers) {
		delete[] renderObjectDataPointer;
	}

	for (GLBufferObject* vbo : vbos) {
		delete vbo;
	}
}

void pcb::Application::run(Application* instance, int argc, char* argv[]) {
	Application::instance = instance;

	initializeGLUT(argc, argv);		// Create OpenGL context before doing anything else OpenGL-related (obviously, but sometimes a necessary reminder).
	loadResources();

	glutMainLoop();
}

void pcb::Application::initializeGLUT(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1280, 1280);
	glutInitWindowPosition(320, 180);

	glutCreateWindow("Procedural City Builder");

	mouseWindowX = glutGet(GLUT_WINDOW_WIDTH) / 2;
	mouseWindowY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
	glutWarpPointer(mouseWindowX, mouseWindowY);

	glutIdleFunc(Application::idleCallback);
	glutDisplayFunc(Application::renderCallback);
	glutReshapeFunc(Application::reshapeCallback);
	glutKeyboardFunc(Application::keyboardCallback);
	glutMotionFunc(Application::mouseMotionCallback);
	glutMouseWheelFunc(Application::mouseWheelCallback);

	glClearColor(0, 0, 1, 0);
	glEnable(GL_DEPTH_TEST);

	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		std::cout << "GLEW INIT FAILED WITH ERROR" << glewError << "\n";
	}
		
	std::string shaderSource = "attribute vec3 aPosition; void main() { gl_Position = vec4(aPosition, 1); }";
	VertexShader defaultVertexShader(shaderSource);
	shaderSource = "void main()	{ gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0); }";
	FragmentShader defaultFragmentShader(shaderSource);
	ShaderProgram defaultShaderProgram(defaultVertexShader, defaultFragmentShader);
	defaultShaderProgram.use();
}

void pcb::Application::loadResources() {
	LayeredTerrainGenerator terrainGenerator(256, 256, 1);
	Terrain* terrain = terrainGenerator.generateNew();

	Image* heightmapImage = terrainGenerator.getHeightmap24BitImageNew();
	heightmapTexture = new Texture(heightmapImage);
	delete heightmapImage;

	Heightmap generatedHeightmap = terrain->generateHeightmap();
	Image* generatedHeightmapImage = generatedHeightmap.to24BitImageNew();
	generatedHeightmapTexture = new Texture(generatedHeightmapImage);
	delete generatedHeightmapImage;

	GLfloat* vertices = new GLfloat[72] {
		-0.25, -0.25, -0.25,
		0.25, -0.25, -0.25,
		0.25, 0.25, -0.25,
		-0.25, 0.25, -0.25,

		-0.25, -0.25, 0.25,
		0.25, -0.25, 0.25,
		0.25, 0.25, 0.25,
		-0.25, 0.25, 0.25,

		-0.25, 0.25, -0.25,
		-0.25, 0.25, 0.25,
		-0.25, -0.25, 0.25,
		-0.25, -0.25, -0.25,

		0.25, 0.25, -0.25,
		0.25, 0.25, 0.25,
		0.25, -0.25, 0.25,
		0.25, -0.25, -0.25,

		-0.25, 0.25, -0.25,
		0.25, 0.25, -0.25,
		0.25, 0.25, 0.25,
		-0.25, 0.25, 0.25,

		-0.25, -0.25, -0.25,
		0.25, -0.25, -0.25,
		0.25, -0.25, 0.25,
		-0.25, -0.25, 0.25
	};

	GLfloat* colors = new GLfloat[72] {
		1, 0, 0,
		1, 0, 0,
		1, 0, 0,
		1, 0, 0,

		0, 1, 0,
		0, 1, 0,
		0, 1, 0,
		0, 1, 0,

		0, 0, 1,
		0, 0, 1,
		0, 0, 1,
		0, 0, 1,

		0.5, 0, 0,
		0.5, 0, 0,
		0.5, 0, 0,
		0.5, 0, 0,

		0, 0.5, 0,
		0, 0.5, 0,
		0, 0.5, 0,
		0, 0.5, 0,

		0, 0, 0.5,
		0, 0, 0.5,
		0, 0, 0.5,
		0, 0, 0.5
	};

	GLfloat* textureCoordinates = new GLfloat[48] {
		0.0, 0.0,
		0.0, 1.0,
		1.0, 1.0,
		1.0, 0.0,

		0.0, 0.0,
		0.0, 1.0,
		1.0, 1.0,
		1.0, 0.0,

		0.0, 0.0,
		0.0, 1.0,
		1.0, 1.0,
		1.0, 0.0,

		0.0, 0.0,
		0.0, 1.0,
		1.0, 1.0,
		1.0, 0.0,

		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,

		0.0, 0.0,
		0.0, 1.0,
		1.0, 1.0,
		1.0, 0.0
	};

	renderObjectsDataPointers[0] = vertices;
	renderObjectsDataPointers[1] = colors;
	renderObjectsDataPointers[2] = textureCoordinates;

	pcb::GLBufferObject* terrainVertices = new GLBufferObject(terrain->getQuadsVertices(), 3, terrain->getQuadsVertexCount());
	vbos.push_back(terrainVertices);
	pcb::GLBufferObject* terrainColors = new GLBufferObject(terrain->getQuadsColors(), 3, terrain->getQuadsVertexCount());
	vbos.push_back(terrainColors);
	pcb::GLBufferObject* heightMapImageObjectVertices = new GLBufferObject(vertices, 3, 24);
	vbos.push_back(heightMapImageObjectVertices);
	pcb::GLBufferObject* heightMapImageObjectTextureCoordinates = new GLBufferObject(textureCoordinates, 2, 24);
	vbos.push_back(heightMapImageObjectTextureCoordinates);

	pcb::SimpleColoredObject* terrainObject = new SimpleColoredObject(*terrainVertices, *terrainColors);
	terrainObject->setPosition(-0.2f, 0, -0.5f);
	pcb::SimpleTexturedObject* heightmapImageObject = new SimpleTexturedObject(*heightMapImageObjectVertices, *heightmapTexture, *heightMapImageObjectTextureCoordinates);
	heightmapImageObject->setPosition(-0.5f, 1, -0.3f);
	pcb::SimpleTexturedObject* generatedHeightmapObject = new SimpleTexturedObject(*heightMapImageObjectVertices, *generatedHeightmapTexture, *heightMapImageObjectTextureCoordinates);
	generatedHeightmapObject->setPosition(-0.5f, 1, 0.3f);

	terrainLayers = static_cast<pcb::LayeredTerrain*>(terrain)->getLayers();
	for (unsigned int i = 0; i < terrainLayers.size(); i++) {
		Terrain& terrainLayer = terrainLayers.at(i);
		pcb::GLBufferObject* terrainlayerVertices = new pcb::GLBufferObject(terrainLayer.getQuadsVertices(), 3, terrainLayer.getQuadsVertexCount());
		vbos.push_back(terrainlayerVertices);
		pcb::GLBufferObject* terrainLayerColors = new pcb::GLBufferObject(terrainLayer.getQuadsColors(), 3, terrainLayer.getQuadsVertexCount());
		vbos.push_back(terrainLayerColors);
		terrainLayerRenderObjects.emplace_back(pcb::SimpleColoredObject(*terrainlayerVertices, *terrainLayerColors));
		SimpleColoredObject& object = terrainLayerRenderObjects.back();
		object.setPosition(-0.2f, static_cast<GLfloat>(i + 1), -0.5f);
	}

	delete terrain;

	renderObjects[0] = terrainObject;
	renderObjects[1] = heightmapImageObject;
	renderObjects[2] = generatedHeightmapObject;
}

void pcb::Application::drawTestShapes() const {
	for (SimpleObject* object : renderObjects) {
		object->render();
	}

	for (const pcb::SimpleColoredObject& terrainLayerObject : terrainLayerRenderObjects) {
		terrainLayerObject.render();
	}
}

void pcb::Application::render() {
	int glutElapsedTime = glutGet(GLUT_ELAPSED_TIME);
	int milisecondsSinceLastRenderStart = glutElapsedTime - previousGlutElapsedTime;
	previousGlutElapsedTime = glutElapsedTime;
	//std::cout << "Time between render calls: " << milisecondsSinceLastRenderStart << ", which would be " << (1.0f / (milisecondsSinceLastRenderStart / 1000.0f)) << " FPS\n";

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glTranslatef(-translationX, -translationY, static_cast<GLfloat>(-2 + zoom));
	glRotatef(90.0f + (0.1f * globalRotationX), 1, 0, 0);
	glRotatef(0.1f * globalRotationY, 0, 1, 0);

	drawTestShapes();

	glPopMatrix();

	glutSwapBuffers();
}

void pcb::Application::reshape(int width, int height) const {
	glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, static_cast<double>(width) / static_cast<double>(height), 0.1, 1000);
	glMatrixMode(GL_MODELVIEW);
}

void pcb::Application::handleKeyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'a':
		translationX -= 0.01f;
		break;
	case 'd':
		translationX += 0.01f;
		break;
	case 'w':
		translationY += 0.01f;
		break;
	case 's':
		translationY -= 0.01f;
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
	zoom += (0.05 * dir);
}