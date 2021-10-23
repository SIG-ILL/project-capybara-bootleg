#include "SimpleObject.hpp"

#include <algorithm>
#include <glm/gtc/type_ptr.hpp>

pcb::SimpleObject::SimpleObject(std::shared_ptr<pcb::VertexPositionBufferObject> vertices) : vertices(vertices), position(0.0f, 0.0f, 0.0f), rotation(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f), modelMatrix(1.0f) {}

pcb::SimpleObject::SimpleObject(const pcb::SimpleObject& other) : vertices(other.vertices), position(other.position), rotation(other.rotation), scale(other.scale), modelMatrix(other.modelMatrix) {}

void pcb::SimpleObject::render() const {
	glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	preRenderAction();

	vertices->enable();
	draw();
	vertices->disable();

	postRenderAction();
}

void pcb::SimpleObject::preRenderAction() const {	/*Empty but defined because virtual*/ }

void pcb::SimpleObject::draw() const {
	glDrawArrays(GL_TRIANGLES, 0, vertices->getVertexCount());
}

void pcb::SimpleObject::postRenderAction() const {	/*Empty but defined becaues virtual*/ }

void pcb::SimpleObject::setPosition(float x, float y, float z) {
	position = glm::vec3(x, y, z);
	updateModelMatrix();
}

void pcb::SimpleObject::setRotation(float x, float y, float z) {
	rotation = glm::vec3(x, y, z);
	updateModelMatrix();
}

void pcb::SimpleObject::setScale(float x, float y, float z) {
	scale = glm::vec3(x, y, z);
	updateModelMatrix();
}

void pcb::SimpleObject::updateModelMatrix() {
	glm::mat4 identityMatrix(1);

	modelMatrix = glm::translate(identityMatrix, position);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1, 0, 0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0, 1, 0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));
	modelMatrix = glm::scale(modelMatrix, scale);
}

pcb::SimpleTexturedObject::SimpleTexturedObject(std::shared_ptr<pcb::VertexPositionBufferObject> vboVertexCoordinates, const pcb::Texture& texture, std::shared_ptr<pcb::VertexTextureCoordinateBufferObject> vboTextureCoordinates) : SimpleObject(vboVertexCoordinates), texture(texture), textureCoordinates(vboTextureCoordinates) {}

pcb::SimpleTexturedObject::SimpleTexturedObject(const pcb::SimpleTexturedObject& other) : SimpleObject(other), texture(other.texture), textureCoordinates(other.textureCoordinates) {}

void pcb::SimpleTexturedObject::preRenderAction() const {
	textureCoordinates->enable();
	glActiveTexture(GL_TEXTURE0);
	texture.bind();
	glUniform1i(3, 0);
}

void pcb::SimpleTexturedObject::postRenderAction() const {
	textureCoordinates->disable();
	texture.unbind();
}

pcb::SimpleColoredObject::SimpleColoredObject(std::shared_ptr<pcb::VertexPositionBufferObject> vboVertexCoordinates, std::shared_ptr<pcb::VertexColorBufferObject> vboVertexColors) : SimpleObject(vboVertexCoordinates), colors(vboVertexColors) {}

pcb::SimpleColoredObject::SimpleColoredObject(const pcb::SimpleColoredObject& other) : SimpleObject(other), colors(other.colors) {}

void pcb::SimpleColoredObject::preRenderAction() const {
	colors->enable();
}

void pcb::SimpleColoredObject::postRenderAction() const {
	colors->disable();
}

pcb::SimpleIndexedColoredObject::SimpleIndexedColoredObject(std::shared_ptr<VertexPositionBufferObject> vboVertexCoordinates, std::shared_ptr<VertexColorBufferObject> vboVertexColors, std::shared_ptr< VertexIndicesBufferObject> vboIndices) : SimpleColoredObject(vboVertexCoordinates, vboVertexColors), indices(vboIndices) {}

void pcb::SimpleIndexedColoredObject::draw() const {
	indices->enable();
	glDrawElements(GL_TRIANGLES, indices->getIndexCount(), GL_UNSIGNED_INT, nullptr);
	indices->disable();
}