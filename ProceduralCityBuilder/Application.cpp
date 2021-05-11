#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Application.hpp"

Application* Application::instance;

void Application::renderCallback() {
	instance->render();
}

void Application::reshapeCallback(int width, int height) {
	instance->reshape(width, height);
}

void Application::keyboardCallback(unsigned char key, int x, int y) {
	instance->handleKeyboard(key, x, y);
}

void Application::idleCallback() {
	instance->idleUpdate();
}

Application::Application() : translationX(0), translationY(0), rotationZ(0), scale(1) {
	
}

void Application::run(Application* instance, int argc, char* argv[]) {
	Application::instance = instance;

	initializeGLUT(argc, argv);
	glutMainLoop();
}

void Application::initializeGLUT(int argc, char* argv[]) {
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

void drawTestShape() {
	GLfloat vertices[] = {
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

	GLfloat colors[]{
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

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);	
	glColorPointer(3, GL_FLOAT, 0, colors);
	glDrawArrays(GL_QUADS, 0, 24);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void Application::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glTranslatef(0, 0, -2);

	glPushMatrix();
	glTranslatef(-1, 0, 0);
	glRotatef(90, 1, 1, 1);
	glScalef(0.75, 0.75, 0.75);
	drawTestShape();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1, 0, 0);
	glRotatef(90, 1, 1, 1);
	glScalef(0.75, 0.75, 0.75);
	drawTestShape();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(translationX, translationY, 0);
	glRotatef(rotationZ, 1, 1, 1);
	glScalef(scale, scale, scale);
	drawTestShape();
	glPopMatrix();

	// Center dot
	glColor3f(0.5f, 0.5f, 0.5f);
	glPushMatrix();
	glScalef(0.025f, 0.025f, 0.025f);
	drawTestShape();
	glPopMatrix();

	glPopMatrix();

	glutSwapBuffers();
}

void Application::reshape(int width, int height) {
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, (float)width/height, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
}

void Application::handleKeyboard(unsigned char key, int x, int y) {
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

void Application::idleUpdate() {
	glutPostRedisplay();
}