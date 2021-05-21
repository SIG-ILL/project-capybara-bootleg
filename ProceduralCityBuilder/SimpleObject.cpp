#include "SimpleObject.hpp"

#include <algorithm>

pcb::SimpleObject::SimpleObject(GLfloat* vertices, GLsizei vertexCount) :vertexCount(vertexCount), vertices(new GLfloat[3 * vertexCount]), x(0), y(0), z(0) {
	std::copy(vertices, vertices + (3 * vertexCount), this->vertices);
}

pcb::SimpleObject::SimpleObject(const pcb::SimpleObject& other) : vertexCount(other.vertexCount), vertices(new GLfloat[3 * other.vertexCount]), x(other.x), y(other.y), z(other.z) {
	std::copy(other.vertices, other.vertices + (3 * other.vertexCount), vertices);
}

pcb::SimpleObject::~SimpleObject() {
	delete[] vertices;
}


void pcb::SimpleObject::render() {
	glPushMatrix();

	glTranslatef(x, y, z);
	//glRotatef(rotationZ, 1, 1, 1);
	//glScalef(scale, scale, scale);

	preRenderAction();

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glDrawArrays(GL_QUADS, 0, vertexCount);
	glDisableClientState(GL_VERTEX_ARRAY);

	postRenderAction();

	glPopMatrix();
}

void pcb::SimpleObject::preRenderAction() {}

void pcb::SimpleObject::postRenderAction() {}

void pcb::SimpleObject::setPosition(GLfloat x, GLfloat y, GLfloat z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

pcb::SimpleTexturedObject::SimpleTexturedObject(GLfloat* vertices, GLsizei vertexCount, Texture& texture, GLfloat* textureCoordinates) : SimpleObject(vertices,vertexCount), texture(texture), textureCoordinates(new GLfloat[2 * vertexCount]) {
	std::copy(textureCoordinates, textureCoordinates + (2 * vertexCount), this->textureCoordinates);
}

pcb::SimpleTexturedObject::SimpleTexturedObject(const pcb::SimpleTexturedObject& other) : SimpleObject(other), texture(other.texture), textureCoordinates(new GLfloat[2 * other.vertexCount]) {
	std::copy(other.textureCoordinates, other.textureCoordinates + (2 * other.vertexCount), this->textureCoordinates);
}

pcb::SimpleTexturedObject::~SimpleTexturedObject() {
	delete[] textureCoordinates;
}

void pcb::SimpleTexturedObject::preRenderAction(){
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, textureCoordinates);

	glEnable(GL_TEXTURE_2D);
	texture.bind();
}

void pcb::SimpleTexturedObject::postRenderAction() {
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
}

pcb::SimpleColoredObject::SimpleColoredObject(GLfloat* vertices, GLsizei vertexCount, GLfloat* colors) : SimpleObject(vertices, vertexCount), colors(new GLfloat[3 * vertexCount]) {
	std::copy(colors, colors + (3 * vertexCount), this->colors);
}

pcb::SimpleColoredObject::SimpleColoredObject(const pcb::SimpleColoredObject& other) : SimpleObject(other), colors(new GLfloat[3 * other.vertexCount]) {
	std::copy(other.colors, other.colors + (3 * other.vertexCount), this->colors);
}

pcb::SimpleColoredObject::~SimpleColoredObject() {
	delete[] colors;
}

void pcb::SimpleColoredObject::preRenderAction() {
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3, GL_FLOAT, 0, colors);
}

void pcb::SimpleColoredObject::postRenderAction() {
	glDisableClientState(GL_COLOR_ARRAY);
}