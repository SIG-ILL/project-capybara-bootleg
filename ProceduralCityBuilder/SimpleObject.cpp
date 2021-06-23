#include "SimpleObject.hpp"

#include <algorithm>
#include <glm/gtc/type_ptr.hpp>

pcb::SimpleObject::SimpleObject(const pcb::VertexPositionBufferObject& vertices) : vertices(vertices), modelMatrix(1) {}

pcb::SimpleObject::SimpleObject(const pcb::SimpleObject& other) : vertices(other.vertices), modelMatrix(other.modelMatrix) {}

void pcb::SimpleObject::render() const {
	glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	preRenderAction();

	vertices.enable();
	glDrawArrays(GL_QUADS, 0, vertices.getVertexCount());
	vertices.disable();

	postRenderAction();
}

void pcb::SimpleObject::preRenderAction() const {}

void pcb::SimpleObject::postRenderAction() const {}

void pcb::SimpleObject::setPosition(GLfloat x, GLfloat y, GLfloat z) {
	modelMatrix = glm::translate(glm::mat4(1), glm::vec3(x, y, z));		// Order: Translate/rotate/scale.
}

pcb::SimpleTexturedObject::SimpleTexturedObject(const pcb::VertexPositionBufferObject& vboVertexCoordinates, const pcb::Texture& texture, const pcb::VertexTextureCoordinateBufferObject& vboTextureCoordinates) : SimpleObject(vboVertexCoordinates), texture(texture), textureCoordinates(vboTextureCoordinates) {}

pcb::SimpleTexturedObject::SimpleTexturedObject(const pcb::SimpleTexturedObject& other) : SimpleObject(other), texture(other.texture), textureCoordinates(other.textureCoordinates) {}

void pcb::SimpleTexturedObject::preRenderAction() const {
	textureCoordinates.enable();
	glActiveTexture(GL_TEXTURE0);
	texture.bind();
	glUniform1i(3, 0);
}

void pcb::SimpleTexturedObject::postRenderAction() const {
	textureCoordinates.disable();
	texture.unbind();
}

pcb::SimpleColoredObject::SimpleColoredObject(const pcb::VertexPositionBufferObject& vboVertexCoordinates, const pcb::VertexColorBufferObject& vboVertexColors) : SimpleObject(vboVertexCoordinates), colors(vboVertexColors) {}

pcb::SimpleColoredObject::SimpleColoredObject(const pcb::SimpleColoredObject& other) : SimpleObject(other), colors(other.colors) {}

void pcb::SimpleColoredObject::preRenderAction() const {
	colors.enable();
}

void pcb::SimpleColoredObject::postRenderAction() const {
	colors.disable();
}