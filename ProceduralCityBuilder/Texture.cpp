#include "Texture.hpp"

pcb::Texture::Texture(Image* sourceImage) : name(0) {
	GLenum pixelDataFormat = GL_RGB;
	switch (sourceImage->getPixelDataFormat()) {
	case PixelDataFormat::BGR:
		pixelDataFormat = GL_BGR;
		break;
	case PixelDataFormat::RGB:
	default:
		pixelDataFormat = GL_RGB;
	}

	glGenTextures(1, &name);
	glActiveTexture(GL_TEXTURE0);
	bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sourceImage->getWidth(), sourceImage->getHeight(), 0, pixelDataFormat, GL_UNSIGNED_BYTE, sourceImage->getPixels());

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unbind();
}

pcb::Texture::~Texture() {
	glDeleteTextures(1, &name);
}

void pcb::Texture::bind() const {
	glBindTexture(GL_TEXTURE_2D, name);
}

void pcb::Texture::unbind() const {
	glBindTexture(GL_TEXTURE_2D, 0);
 }