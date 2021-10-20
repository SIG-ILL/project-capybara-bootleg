#include "VertexBufferObject.hpp"

#include <algorithm>

pcb::VertexBufferObject::VertexBufferObject(GLuint attributeIndex, std::shared_ptr<std::vector<GLfloat>> vertexData, int elementsPerVertex, int vertexCount) : attributeIndex(attributeIndex), name(0), elementsPerVertex(elementsPerVertex), vertexCount(vertexCount) {
	glGenBuffers(1, &name);
	bind();
	glBufferData(GL_ARRAY_BUFFER, elementsPerVertex * vertexCount * sizeof(GLfloat), vertexData->data(), GL_STATIC_DRAW);
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

pcb::VertexPositionBufferObject::VertexPositionBufferObject(std::shared_ptr<std::vector<GLfloat>> vertexData, int vertexCount) : VertexBufferObject(static_cast<int>(VertexAttribute::Position), vertexData, 3, vertexCount) {}

pcb::VertexColorBufferObject::VertexColorBufferObject(std::shared_ptr<std::vector<GLfloat>> vertexData, int elementsPerVertex, int vertexCount) : VertexBufferObject(static_cast<int>(VertexAttribute::Color), vertexData, elementsPerVertex, vertexCount) {}

pcb::VertexTextureCoordinateBufferObject::VertexTextureCoordinateBufferObject(std::shared_ptr<std::vector<GLfloat>> vertexData, int vertexCount) : VertexBufferObject(static_cast<int>(VertexAttribute::TextureCoordinate), vertexData, 2, vertexCount) {}

pcb::VertexIndicesBufferObject::VertexIndicesBufferObject(std::shared_ptr<std::vector<unsigned int>> indexData, int indexCount) : indexCount(indexCount) {
	glGenBuffers(1, &name);
	enable();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData->size() * sizeof(unsigned int), indexData->data(), GL_STATIC_DRAW);
	disable();
}

void pcb::VertexIndicesBufferObject::enable() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, name);
}

void pcb::VertexIndicesBufferObject::disable() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

int pcb::VertexIndicesBufferObject::getIndexCount() const {
	return indexCount;
}