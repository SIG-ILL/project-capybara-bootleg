#include "Image.hpp"

pcb::Image::Image(char const* sourcePixels, int pixelArraySize, int width, int height, pcb::PixelDataFormat pixelDataFormat) : pixelArray(new char[pixelArraySize]), widthInPixels(width), heightInPixels(height), pixelDataFormat(pixelDataFormat) {
	for (int i = 0; i < pixelArraySize; i++) {
		pixelArray[i] = sourcePixels[i];
	}
}

pcb::Image::~Image() {
	delete[] pixelArray;
}

int pcb::Image::getWidth() const { return widthInPixels; }

int pcb::Image::getHeight() const { return heightInPixels; }

char* pcb::Image::getPixels() const { return pixelArray; }

pcb::PixelDataFormat pcb::Image::getPixelDataFormat() const { return pixelDataFormat; }