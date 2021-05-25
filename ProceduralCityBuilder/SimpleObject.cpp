#include "SimpleObject.hpp"

#include <algorithm>

pcb::SimpleObject::SimpleObject(const pcb::GLBufferObject& vertices) : vertices(vertices), x(0), y(0), z(0) {}

pcb::SimpleObject::SimpleObject(const pcb::SimpleObject& other) : vertices(other.vertices), x(other.x), y(other.y), z(other.z) {}

void pcb::SimpleObject::render() const {
	glPushMatrix();

	glTranslatef(x, y, z);
	//glRotatef(rotationZ, 1, 1, 1);
	//glScalef(scale, scale, scale);

	preRenderAction();

	vertices.bind();
	vertices.enableAndSet();
	glDrawArrays(GL_QUADS, 0, vertices.getVertexCount());
	vertices.disable();

	postRenderAction();

	glPopMatrix();
}

void pcb::SimpleObject::preRenderAction() const {}

void pcb::SimpleObject::postRenderAction() const {}

void pcb::SimpleObject::setPosition(GLfloat x, GLfloat y, GLfloat z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

pcb::SimpleTexturedObject::SimpleTexturedObject(const pcb::GLBufferObject& vboVertexCoordinates, const Texture& texture, const pcb::GLBufferObject& vboTextureCoordinates) : SimpleObject(vboVertexCoordinates), texture(texture), textureCoordinates(vboTextureCoordinates) {}

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

pcb::SimpleColoredObject::SimpleColoredObject(const pcb::GLBufferObject& vboVertexCoordinates, const pcb::GLBufferObject& vboVertexColors) : SimpleObject(vboVertexCoordinates), colors(vboVertexColors) {}

pcb::SimpleColoredObject::SimpleColoredObject(const pcb::SimpleColoredObject& other) : SimpleObject(other), colors(other.colors) {}

void pcb::SimpleColoredObject::preRenderAction() const {
	/*glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3, GL_FLOAT, 0, colors);*/
}

void pcb::SimpleColoredObject::postRenderAction() const {
	/*glDisableClientState(GL_COLOR_ARRAY);*/
}