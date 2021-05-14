#pragma once

#include <GL/glew.h>

#include "Texture.hpp"

namespace pcb {
	class SimpleObject {
	public:
		SimpleObject(GLfloat* vertices, GLsizei vertexCount);

		virtual void render();
		void setPosition(GLfloat x, GLfloat y, GLfloat z);

	protected:
		virtual void postRenderAction();		

	private:
		GLfloat* vertices;
		GLsizei vertexCount;
		GLfloat x;
		GLfloat y;
		GLfloat z;
	};

	class SimpleTexturedObject : public SimpleObject {
	public:
		SimpleTexturedObject(GLfloat* vertices, GLsizei vertexCount, Texture& texture, GLfloat* textureCoordinates);

		virtual void render() override;

	protected:
		virtual void postRenderAction() override;

	private:
		Texture& texture;
		GLfloat* textureCoordinates;
	};

	class SimpleColoredObject : public SimpleObject {
	public:
		SimpleColoredObject(GLfloat* vertices, GLsizei vertexCount, GLfloat* colors);

		virtual void render() override;

	protected:
		virtual void postRenderAction() override;

	private:
		GLfloat* colors;
	};
}