#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Texture.hpp"
#include "VertexBufferObject.hpp"

namespace pcb {
	class SimpleObject {
	public:
		SimpleObject(const pcb::VertexPositionBufferObject& vertices);
		SimpleObject(const pcb::SimpleObject& other);

		void render() const;
		void setPosition(GLfloat x, GLfloat y, GLfloat z);

	protected:
		virtual void preRenderAction() const;
		virtual void postRenderAction() const;

	private:
		const pcb::VertexPositionBufferObject& vertices;
		glm::mat4 modelMatrix;
	};

	class SimpleTexturedObject final : public SimpleObject {
	public:
		SimpleTexturedObject(const pcb::VertexPositionBufferObject& vboVertexCoordinates, const pcb::Texture& texture, const pcb::VertexTextureCoordinateBufferObject& VertexTextureCoordinateBufferObject);
		SimpleTexturedObject(const pcb::SimpleTexturedObject& other);

	protected:
		void preRenderAction() const override;
		void postRenderAction() const override;

	private:
		const Texture& texture;
		const pcb::VertexTextureCoordinateBufferObject& textureCoordinates;
	};

	class SimpleColoredObject final : public SimpleObject {
	public:
		SimpleColoredObject(const pcb::VertexPositionBufferObject& vboVertexCoordinates, const pcb::VertexColorBufferObject& vboVertexColors);
		SimpleColoredObject(const pcb::SimpleColoredObject& other);

	protected:
		void preRenderAction() const override;
		void postRenderAction() const override;

	private:
		const pcb::VertexColorBufferObject& colors;
	};
}