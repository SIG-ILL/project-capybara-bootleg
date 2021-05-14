#include "Image.hpp"

pcb::Image::Image(char* sourcePixels, int pixelArraySize, int width, int height, PixelDataFormat pixelDataFormat) : pixelArray(new char[pixelArraySize]), widthInPixels(width), heightInPixels(height), pixelDataFormat(pixelDataFormat) {
	for (int i = 0; i < pixelArraySize; i++) {
		pixelArray[i] = sourcePixels[i];
	}
}

pcb::Image::~Image() {
	delete[] pixelArray;
}

int pcb::Image::getWidth() { return widthInPixels; }

int pcb::Image::getHeight() { return heightInPixels; }

char* pcb::Image::getPixels() { return pixelArray; }

pcb::PixelDataFormat pcb::Image::getPixelDataFormat() { return pixelDataFormat; }