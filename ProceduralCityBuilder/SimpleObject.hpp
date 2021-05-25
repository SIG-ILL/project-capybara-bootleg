#pragma once

#include <GL/glew.h>

#include "Texture.hpp"
#include "GLBufferObject.hpp"

namespace pcb {
	class SimpleObject {
	public:
		SimpleObject(const pcb::GLBufferObject& vertices);
		SimpleObject(const pcb::SimpleObject& other);

		void render() const;
		void setPosition(GLfloat x, GLfloat y, GLfloat z);

	protected:
		virtual void preRenderAction() const;
		virtual void postRenderAction() const;

	private:
		const pcb::GLBufferObject& vertices;
		GLfloat x;
		GLfloat y;
		GLfloat z;
	};

	class SimpleTexturedObject final : public SimpleObject {
	public:
		SimpleTexturedObject(const pcb::GLBufferObject& vboVertexCoordinates, const Texture& texture, const pcb::GLBufferObject& vboTextureCoordinates);
		SimpleTexturedObject(const pcb::SimpleTexturedObject& other);

	protected:
		void preRenderAction() const override;
		void postRenderAction() const override;

	private:
		const Texture& texture;
		const pcb::GLBufferObject& textureCoordinates;
	};

	class SimpleColoredObject final : public SimpleObject {
	public:
		SimpleColoredObject(const pcb::GLBufferObject& vboVertexCoordinates, const pcb::GLBufferObject& vboVertexColors);
		SimpleColoredObject(const pcb::SimpleColoredObject& other);

	protected:
		void preRenderAction() const override;
		void postRenderAction() const override;

	private:
		const pcb::GLBufferObject& colors;
	};
}