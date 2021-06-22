#include "VertexArrayObject.hpp"

pcb::VertexArrayObject::VertexArrayObject() {
	glGenVertexArrays(1, &name);
	bind();
}

pcb::VertexArrayObject::~VertexArrayObject() {
	glDeleteVertexArrays(1, &name);
}

void pcb::VertexArrayObject::bind() const {
	glBindVertexArray(name);
}