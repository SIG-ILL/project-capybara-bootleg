#include "GLBufferObject.hpp"

#include <algorithm>

pcb::GLBufferObject::GLBufferObject(GLfloat* vertices, int elementsPerVertex, int vertexCount) : name(0), elementsPerVertex(elementsPerVertex), vertexCount(vertexCount) {
	glGenBuffers(1, &name);
	bind();
	glBufferData(GL_ARRAY_BUFFER, elementsPerVertex * vertexCount * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
}

pcb::GLBufferObject::~GLBufferObject() {
	glDeleteBuffers(1, &name);
}

void pcb::GLBufferObject::bind() const {
	glBindBuffer(GL_ARRAY_BUFFER, name);
}

void pcb::GLBufferObject::enableAndSet() const {
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, elementsPerVertex, GL_FLOAT, GL_FALSE, 0, 0);
}

void pcb::GLBufferObject::disable() const {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);
}

int pcb::GLBufferObject::getVertexCount() const {
	return vertexCount;
}