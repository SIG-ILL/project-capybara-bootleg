#pragma once

#include <GL/glew.h>

#include "Texture.hpp"

namespace pcb {
	class SimpleObject {
	public:
		SimpleObject(GLfloat* vertices, GLsizei vertexCount);
		SimpleObject(const pcb::SimpleObject& other);
		virtual ~SimpleObject();

		void render();
		void setPosition(GLfloat x, GLfloat y, GLfloat z);

	protected:
		GLsizei vertexCount;

		virtual void preRenderAction();
		virtual void postRenderAction();		

	private:
		GLfloat* vertices;		
		GLfloat x;
		GLfloat y;
		GLfloat z;
	};

	class SimpleTexturedObject final : public SimpleObject {
	public:
		SimpleTexturedObject(GLfloat* vertices, GLsizei vertexCount, Texture& texture, GLfloat* textureCoordinates);
		SimpleTexturedObject(const pcb::SimpleTexturedObject& other);
		~SimpleTexturedObject();

	protected:
		void preRenderAction() override;
		void postRenderAction() override;

	private:
		Texture& texture;
		GLfloat* textureCoordinates;
	};

	class SimpleColoredObject final : public SimpleObject {
	public:
		SimpleColoredObject(GLfloat* vertices, GLsizei vertexCount, GLfloat* colors);
		SimpleColoredObject(const pcb::SimpleColoredObject& other);
		~SimpleColoredObject();

	protected:
		void preRenderAction() override;
		void postRenderAction() override;

	private:
		GLfloat* colors;
	};
}