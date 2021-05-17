#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Application.hpp"
#include "BitmapLoader.hpp"
#include "TerrainGenerator.hpp"

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

pcb::Application::Application() : translationX(0), translationY(0), rotationZ(0), scale(1), testTexture(nullptr), renderObjects{ nullptr, nullptr, nullptr }, renderObjectsDataPointers{ nullptr, nullptr, nullptr } {}

pcb::Application::~Application() {
	delete testTexture;

	for (SimpleObject* object : renderObjects) {
		delete object;
	}

	for (GLfloat* renderObjectDataPointer : renderObjectsDataPointers) {
		delete[] renderObjectDataPointer;
	}
}

void pcb::Application::run(Application* instance, int argc, char* argv[]) {
	Application::instance = instance;

	initializeGLUT(argc, argv);		// Create OpenGL context before doing anything else OpenGL-related (obviously, but sometimes a necessary reminder).
	loadResources();



	//pcb::TerrainGenerator terrainGenerator;
	//terrainGenerator.generate();




	glutMainLoop();
}

void pcb::Application::initializeGLUT(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1280, 1280);
	glutInitWindowPosition(320, 180);

	glutCreateWindow("Procedural City Builder");

	glutIdleFunc(Application::idleCallback);
	glutDisplayFunc(Application::renderCallback);
	glutReshapeFunc(Application::reshapeCallback);
	glutKeyboardFunc(Application::keyboardCallback);

	glEnable(GL_DEPTH_TEST);
}

void pcb::Application::loadResources() {
	//pcb::BitmapLoader imageLoader;
	//Image* textureImage = imageLoader.loadFromFile("test2.bmp");
	pcb::HeightMapGenerator heightMapGenerator(256, 256);
	HeightMap* heightMap = heightMapGenerator.generate();
	Image* textureImage = heightMap->to24BitImage();

	testTexture = new Texture(textureImage);
	delete textureImage;
	delete heightMap;

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
		0.0, 1.0,
		1.0, 1.0,
		1.0, 0.0,

		0.0, 0.0,
		0.0, 1.0,
		1.0, 1.0,
		1.0, 0.0
	};

	renderObjectsDataPointers[0] = vertices;
	renderObjectsDataPointers[1] = colors;
	renderObjectsDataPointers[2] = textureCoordinates;

	pcb::SimpleObject* simpleObject = new SimpleObject(vertices, 24);
	simpleObject->setPosition(-1, 0, 0);
	pcb::SimpleColoredObject* coloredObject = new SimpleColoredObject(vertices, 24, colors);
	pcb::SimpleTexturedObject* texturedObject = new SimpleTexturedObject(vertices, 24, *testTexture, textureCoordinates);
	//texturedObject->setPosition(1, 0, 0);
	texturedObject->setPosition(0, 0, 1.4);

	renderObjects[0] = simpleObject;
	renderObjects[1] = coloredObject;
	renderObjects[2] = texturedObject;
}

void pcb::Application::drawTestShapes() {
	renderObjects[1]->setPosition(translationX, translationY, 0);

	for (SimpleObject* object : renderObjects) {
		object->render();
	}
}

void pcb::Application::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glTranslatef(0, 0, -2);

	drawTestShapes();

	/*glPushMatrix();
	glTranslatef(-1, 0, 0);
	glRotatef(90, 1, 1, 1);
	glScalef(0.75, 0.75, 0.75);
	drawTestShapes();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1, 0, 0);
	glRotatef(90, 1, 1, 1);
	glScalef(0.75, 0.75, 0.75);
	drawTestShapes();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(translationX, translationY, 0);
	glRotatef(rotationZ, 1, 1, 1);
	glScalef(scale, scale, scale);
	drawTestShapes();
	glPopMatrix();

	// Center dot
	glColor3f(0.5f, 0.5f, 0.5f);
	glPushMatrix();
	glScalef(0.025f, 0.025f, 0.025f);
	drawTestShapes();
	glPopMatrix();*/

	glPopMatrix();

	glutSwapBuffers();
}

void pcb::Application::reshape(int width, int height) {
	glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, static_cast<float>(width) / height, 0.1, 1000);
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

void pcb::Application::idleUpdate() {
	glutPostRedisplay();
}