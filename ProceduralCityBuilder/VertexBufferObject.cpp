#include "VertexBufferObject.hpp"

#include <algorithm>

pcb::VertexBufferObject::VertexBufferObject(GLuint attributeIndex, const GLfloat* const vertexData, int elementsPerVertex, int vertexCount) : attributeIndex(attributeIndex), name(0), elementsPerVertex(elementsPerVertex), vertexCount(vertexCount) {
	glGenBuffers(1, &name);
	bind();
	glBufferData(GL_ARRAY_BUFFER, elementsPerVertex * vertexCount * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);
}

pcb::VertexBufferObject::~VertexBufferObject() {
	glDeleteBuffers(1, &name);
}

void pcb::VertexBufferObject::bind() const {
	glBindBuffer(GL_ARRAY_BUFFER, name);
}

void pcb::VertexBufferObject::enable() const {
	bind();
	glEnableVertexAttribArray(attributeIndex);
	glVertexAttribPointer(attributeIndex, elementsPerVertex, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void pcb::VertexBufferObject::disable() const {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(attributeIndex);
}

int pcb::VertexBufferObject::getVertexCount() const {
	return vertexCount;
}

pcb::VertexPositionBufferObject::VertexPositionBufferObject(const GLfloat* const vertexData, int vertexCount) : VertexBufferObject(static_cast<int>(VertexAttribute::Position), vertexData, 3, vertexCount) {}

pcb::VertexColorBufferObject::VertexColorBufferObject(const GLfloat* const vertexData, int elementsPerVertex, int vertexCount) : VertexBufferObject(static_cast<int>(VertexAttribute::Color), vertexData, elementsPerVertex, vertexCount) {}

pcb::VertexTextureCoordinateBufferObject::VertexTextureCoordinateBufferObject(const GLfloat* const vertexData, int vertexCount) : VertexBufferObject(static_cast<int>(VertexAttribute::TextureCoordinate), vertexData, 2, vertexCount) {}