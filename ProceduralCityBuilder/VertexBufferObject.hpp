#pragma once

#include <GL/glew.h>

namespace pcb {
	enum class VertexAttribute { Position = 0, Color = 1, TextureCoordinate = 2 };

	class VertexBufferObject {
	protected:
		VertexBufferObject(GLuint attributeIndex, const GLfloat* const vertexData, int elementsPerVertex, int vertexCount);

	public:
		virtual ~VertexBufferObject();

		void enable() const;
		void disable() const;
		int getVertexCount() const;

	private:
		GLuint attributeIndex;
		GLuint name;
		int elementsPerVertex;
		int vertexCount;

		void bind() const;
	};

	class VertexPositionBufferObject : public VertexBufferObject {
	public:
		VertexPositionBufferObject(const GLfloat* const vertexData, int vertexCount);
	};

	class VertexColorBufferObject : public VertexBufferObject {
	public:
		VertexColorBufferObject(const GLfloat* const vertexData, int elementsPerVertex, int vertexCount);
	};

	class VertexTextureCoordinateBufferObject : public VertexBufferObject {
	public:
		VertexTextureCoordinateBufferObject(const GLfloat* const vertexData, int vertexCount);
	};
}