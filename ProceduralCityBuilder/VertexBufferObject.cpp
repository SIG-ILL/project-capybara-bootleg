#include "VertexBufferObject.hpp"

#include <algorithm>

pcb::VertexBufferObject::VertexBufferObject(GLfloat* vertices, int elementsPerVertex, int vertexCount) : name(0), elementsPerVertex(elementsPerVertex), vertexCount(vertexCount) {
	glGenBuffers(1, &name);
	bind();
	glBufferData(GL_ARRAY_BUFFER, elementsPerVertex * vertexCount * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
}

pcb::VertexBufferObject::~VertexBufferObject() {
	glDeleteBuffers(1, &name);
}

void pcb::VertexBufferObject::bind() const {
	glBindBuffer(GL_ARRAY_BUFFER, name);
}

void pcb::VertexBufferObject::enableAndSet() const {
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, elementsPerVertex, GL_FLOAT, GL_FALSE, 0, 0);
}

void pcb::VertexBufferObject::disable() const {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);
}

int pcb::VertexBufferObject::getVertexCount() const {
	return vertexCount;
}

pcb::VertexColorBufferObject::VertexColorBufferObject(GLfloat* vertexColors, int elementsPerVertex, int vertexCount) : name(0), elementsPerVertex(elementsPerVertex), vertexCount(vertexCount) {
	glGenBuffers(1, &name);
	bind();
	glBufferData(GL_ARRAY_BUFFER, elementsPerVertex * vertexCount * sizeof(GLfloat), vertexColors, GL_STATIC_DRAW);
}

pcb::VertexColorBufferObject::~VertexColorBufferObject() {
	glDeleteBuffers(1, &name);
}

void pcb::VertexColorBufferObject::bind() const {
	glBindBuffer(GL_ARRAY_BUFFER, name);
}

void pcb::VertexColorBufferObject::enableAndSet() const {
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, elementsPerVertex, GL_FLOAT, GL_FALSE, 0, 0);
}

void pcb::VertexColorBufferObject::disable() const {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(1);
}

int pcb::VertexColorBufferObject::getVertexCount() const {
	return vertexCount;
}