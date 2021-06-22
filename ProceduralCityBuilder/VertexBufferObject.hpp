#pragma once

#include <GL/glew.h>

namespace pcb {
	class VertexBufferObject {
	public:
		VertexBufferObject(GLfloat* vertices, int elementsPerVertex, int vertexCount);
		~VertexBufferObject();

		void bind() const;
		void enableAndSet() const;
		void disable() const;
		int getVertexCount() const;

	private:
		GLuint name;
		int elementsPerVertex;
		int vertexCount;
	};

	class VertexColorBufferObject {
	public:
		VertexColorBufferObject(GLfloat* vertexColors, int elementsPerVertex, int vertexCount);
		~VertexColorBufferObject();

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