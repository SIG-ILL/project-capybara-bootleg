#pragma once

#include <GL/glew.h>
#include <memory>
#include <vector>

namespace pcb {
	enum class VertexAttribute { Position = 0, Color = 1, TextureCoordinate = 2 };

	class VertexBufferObject {
	protected:
		VertexBufferObject(GLuint attributeIndex, std::shared_ptr<std::vector<GLfloat>> vertexData, int elementsPerVertex, int vertexCount);

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
		VertexPositionBufferObject(std::shared_ptr<std::vector<GLfloat>> vertexData, int vertexCount);
	};

	class VertexColorBufferObject : public VertexBufferObject {
	public:
		VertexColorBufferObject(std::shared_ptr<std::vector<GLfloat>> vertexData, int elementsPerVertex, int vertexCount);
	};

	class VertexTextureCoordinateBufferObject : public VertexBufferObject {
	public:
		VertexTextureCoordinateBufferObject(std::shared_ptr<std::vector<GLfloat>> vertexData, int vertexCount);
	};

	class VertexIndicesBufferObject final {
	public:
		VertexIndicesBufferObject(std::shared_ptr<std::vector<unsigned int>> indexData, int indexCount);

		void enable() const;
		void disable() const;

		int getIndexCount() const;

	private:
		GLuint name;
		int indexCount;
	};
}