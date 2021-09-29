#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>

#include "Texture.hpp"
#include "VertexBufferObject.hpp"

namespace pcb {
	class SimpleObject {
	public:
		SimpleObject(std::shared_ptr<VertexPositionBufferObject> vertices);
		SimpleObject(const SimpleObject& other);

		void render() const;
		void setPosition(float x, float y, float z);
		void setRotation(float x, float y, float z);
		void setScale(float x, float y, float z);

	protected:
		virtual void preRenderAction() const;
		virtual void postRenderAction() const;

	private:
		std::shared_ptr<VertexPositionBufferObject> vertices;
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;
		glm::mat4 modelMatrix;

		void updateModelMatrix();
	};

	class SimpleTexturedObject final : public SimpleObject {
	public:
		SimpleTexturedObject(std::shared_ptr<VertexPositionBufferObject> vboVertexCoordinates, const Texture& texture, std::shared_ptr<VertexTextureCoordinateBufferObject> VertexTextureCoordinateBufferObject);
		SimpleTexturedObject(const SimpleTexturedObject& other);

	protected:
		void preRenderAction() const override;
		void postRenderAction() const override;

	private:
		const Texture& texture;
		std::shared_ptr<VertexTextureCoordinateBufferObject> textureCoordinates;
	};

	class SimpleColoredObject final : public SimpleObject {
	public:
		SimpleColoredObject(std::shared_ptr<VertexPositionBufferObject> vboVertexCoordinates, std::shared_ptr<VertexColorBufferObject> vboVertexColors);
		SimpleColoredObject(const SimpleColoredObject& other);

	protected:
		void preRenderAction() const override;
		void postRenderAction() const override;

	private:
		std::shared_ptr<VertexColorBufferObject> colors;
	};
}