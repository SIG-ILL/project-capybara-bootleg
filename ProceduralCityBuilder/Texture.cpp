#include "Texture.hpp"

pcb::Texture::Texture(Image* sourceImage) : id(0) {
	GLenum pixelDataFormat = GL_RGB;
	switch (sourceImage->getPixelDataFormat()) {
	case PixelDataFormat::BGR:
		pixelDataFormat = GL_BGR;
		break;
	case PixelDataFormat::RGB:
	default:
		pixelDataFormat = GL_RGB;
	}

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sourceImage->getWidth(), sourceImage->getHeight(), 0, pixelDataFormat, GL_UNSIGNED_BYTE, sourceImage->getPixels());

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

pcb::Texture::~Texture() {
	glDeleteTextures(1, &id);
}

void pcb::Texture::bind() {
	glBindTexture(GL_TEXTURE_2D, id);
}