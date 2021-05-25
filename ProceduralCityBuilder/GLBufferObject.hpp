#pragma once

#include <GL/glew.h>

namespace pcb {
	class GLBufferObject {
	public:
		GLBufferObject(GLfloat* vertices, int elementsPerVertex, int vertexCount);
		~GLBufferObject();

		void bind() const;
		void enableAndSet() const;
		void disable() const;
		int getVertexCount() const;

	private:
		GLuint name;
		int elementsPerVertex;
		int vertexCount;
	};
}