#include "SimpleObject.hpp"

#include <algorithm>
#include <glm/gtc/type_ptr.hpp>

pcb::SimpleObject::SimpleObject(const pcb::VertexBufferObject& vertices) : vertices(vertices), modelMatrix(1) {}

pcb::SimpleObject::SimpleObject(const pcb::SimpleObject& other) : vertices(other.vertices), modelMatrix(other.modelMatrix) {}

void pcb::SimpleObject::render() const {
	glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	preRenderAction();

	vertices.bind();
	vertices.enableAndSet();
	glDrawArrays(GL_QUADS, 0, vertices.getVertexCount());
	vertices.disable();

	postRenderAction();
}

void pcb::SimpleObject::preRenderAction() const {}

void pcb::SimpleObject::postRenderAction() const {}

void pcb::SimpleObject::setPosition(GLfloat x, GLfloat y, GLfloat z) {
	modelMatrix = glm::translate(glm::mat4(1), glm::vec3(x, y, z));		// Order: Translate/rotate/scale.
}

pcb::SimpleTexturedObject::SimpleTexturedObject(const pcb::VertexBufferObject& vboVertexCoordinates, const Texture& texture, const pcb::VertexBufferObject& vboTextureCoordinates) : SimpleObject(vboVertexCoordinates), texture(texture), textureCoordinates(vboTextureCoordinates) {}

pcb::SimpleTexturedObject::SimpleTexturedObject(const pcb::SimpleTexturedObject& other) : SimpleObject(other), texture(other.texture), textureCoordinates(other.textureCoordinates) {}

void pcb::SimpleTexturedObject::preRenderAction() const {
	/*glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, textureCoordinates);

	glEnable(GL_TEXTURE_2D);
	texture.bind();*/
}

void pcb::SimpleTexturedObject::postRenderAction() const {
	/*glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);*/
}

pcb::SimpleColoredObject::SimpleColoredObject(const pcb::VertexBufferObject& vboVertexCoordinates, const pcb::VertexBufferObject& vboVertexColors) : SimpleObject(vboVertexCoordinates), colors(vboVertexColors) {}

pcb::SimpleColoredObject::SimpleColoredObject(const pcb::SimpleColoredObject& other) : SimpleObject(other), colors(other.colors) {}

void pcb::SimpleColoredObject::preRenderAction() const {
	/*glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3, GL_FLOAT, 0, colors);*/
}

void pcb::SimpleColoredObject::postRenderAction() const {
	/*glDisableClientState(GL_COLOR_ARRAY);*/
}