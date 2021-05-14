#include "SimpleObject.hpp"

pcb::SimpleObject::SimpleObject(GLfloat* vertices, GLsizei vertexCount) : vertices(vertices), vertexCount(vertexCount), x(0), y(0), z(0) {}


void pcb::SimpleObject::render() {
	glPushMatrix();

	glTranslatef(x, y, z);
	//glRotatef(rotationZ, 1, 1, 1);
	//glScalef(scale, scale, scale);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glDrawArrays(GL_QUADS, 0, vertexCount);
	glDisableClientState(GL_VERTEX_ARRAY);

	postRenderAction();

	glPopMatrix();
}

void pcb::SimpleObject::postRenderAction() {}

void pcb::SimpleObject::setPosition(GLfloat x, GLfloat y, GLfloat z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

pcb::SimpleTexturedObject::SimpleTexturedObject(GLfloat* vertices, GLsizei vertexCount, Texture& texture, GLfloat* textureCoordinates) : SimpleObject(vertices,vertexCount), texture(texture), textureCoordinates(textureCoordinates) {}

void pcb::SimpleTexturedObject::render() {
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, textureCoordinates);

	glEnable(GL_TEXTURE_2D);
	texture.bind();

	SimpleObject::render();

	glDisable(GL_TEXTURE_2D);
}

void pcb::SimpleTexturedObject::postRenderAction() {

}

pcb::SimpleColoredObject::SimpleColoredObject(GLfloat* vertices, GLsizei vertexCount, GLfloat* colors) : SimpleObject(vertices, vertexCount), colors(colors) {}

void pcb::SimpleColoredObject::render() {
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3, GL_FLOAT, 0, colors);

	SimpleObject::render();

}

void pcb::SimpleColoredObject::postRenderAction() {
	glDisableClientState(GL_COLOR_ARRAY);
}