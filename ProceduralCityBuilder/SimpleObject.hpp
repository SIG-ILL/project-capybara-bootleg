#pragma once

#include <GL/glew.h>

#include "Texture.hpp"

namespace pcb {
	class SimpleObject {
	public:
		SimpleObject(GLfloat* vertices, GLsizei vertexCount);
		SimpleObject(const pcb::SimpleObject& other);
		virtual ~SimpleObject();

		void render() const;
		void setPosition(GLfloat x, GLfloat y, GLfloat z);

	protected:
		GLsizei vertexCount;

		virtual void preRenderAction() const;
		virtual void postRenderAction() const;

	private:
		GLfloat* vertices;		
		GLfloat x;
		GLfloat y;
		GLfloat z;
	};

	class SimpleTexturedObject final : public SimpleObject {
	public:
		SimpleTexturedObject(GLfloat* vertices, GLsizei vertexCount, const Texture& texture, GLfloat* textureCoordinates);
		SimpleTexturedObject(const pcb::SimpleTexturedObject& other);
		~SimpleTexturedObject();

	protected:
		void preRenderAction() const override;
		void postRenderAction() const override;

	private:
		const Texture& texture;
		GLfloat* textureCoordinates;
	};

	class SimpleColoredObject final : public SimpleObject {
	public:
		SimpleColoredObject(GLfloat* vertices, GLsizei vertexCount, GLfloat* colors);
		SimpleColoredObject(const pcb::SimpleColoredObject& other);
		~SimpleColoredObject();

	protected:
		void preRenderAction() const override;
		void postRenderAction() const override;

	private:
		GLfloat* colors;
	};
}