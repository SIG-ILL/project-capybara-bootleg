#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Texture.hpp"
#include "VertexBufferObject.hpp"

namespace pcb {
	class SimpleObject {
	public:
		SimpleObject(const pcb::VertexBufferObject& vertices);
		SimpleObject(const pcb::SimpleObject& other);

		void render() const;
		void setPosition(GLfloat x, GLfloat y, GLfloat z);

	protected:
		virtual void preRenderAction() const;
		virtual void postRenderAction() const;

	private:
		const pcb::VertexBufferObject& vertices;
		glm::mat4 modelMatrix;
	};

	class SimpleTexturedObject final : public SimpleObject {
	public:
		SimpleTexturedObject(const pcb::VertexBufferObject& vboVertexCoordinates, const Texture& texture, const pcb::VertexBufferObject& vboTextureCoordinates);
		SimpleTexturedObject(const pcb::SimpleTexturedObject& other);

	protected:
		void preRenderAction() const override;
		void postRenderAction() const override;

	private:
		const Texture& texture;
		const pcb::VertexBufferObject& textureCoordinates;
	};

	class SimpleColoredObject final : public SimpleObject {
	public:
		SimpleColoredObject(const pcb::VertexBufferObject& vboVertexCoordinates, const pcb::VertexBufferObject& vboVertexColors);
		SimpleColoredObject(const pcb::SimpleColoredObject& other);

	protected:
		void preRenderAction() const override;
		void postRenderAction() const override;

	private:
		const pcb::VertexBufferObject& colors;
	};
}