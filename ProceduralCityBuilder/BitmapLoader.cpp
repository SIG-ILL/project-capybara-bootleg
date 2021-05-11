#include <fstream>

#include "BitmapLoader.hpp"
#include "BitshiftingUtility.hpp"

Image::Image() {

}

Image::~Image() {

}

Image* BitmapLoader::loadFromFile(std::string filename) {
	std::ifstream fileStream;
	fileStream.open(filename, std::fstream::binary);
	if (!fileStream.is_open()) {
		return nullptr;
	}

	char bufferBitMapHeader[2];
	fileStream.read(bufferBitMapHeader, 2);
	if (bufferBitMapHeader[0] != 'B' || bufferBitMapHeader[1] != 'M') {
		fileStream.close();
		return nullptr;
	}

	fileStream.ignore(8);

	char bufferInt[4];
	fileStream.read(bufferInt, 4);
	int pixelArrayOffset = (int)(((unsigned char)bufferInt[3] << 24) | ((unsigned char)bufferInt[2] << 16) | ((unsigned char)bufferInt[1] << 8) | (unsigned char)bufferInt[0]);
	//int pixelArrayOffset = littleToBigEndian<int>(bufferInt, 4);

	fileStream.read(bufferInt, 4);
	int headerSize = (unsigned int)(((unsigned char)bufferInt[3] << 24) | ((unsigned char)bufferInt[2] << 16) | ((unsigned char)bufferInt[1] << 8) | (unsigned char)bufferInt[0]);

	int bitmapWidth;
	int bitmapHeight;
	short colorDepth;

	switch (headerSize) {
	case 12:

		break;
	case 64:

		break;
	case 16:

		break;
	case 124:
	case 108:	
	case 40:
		fileStream.read(bufferInt, 4);
		bitmapWidth = (int)(((unsigned char)bufferInt[3] << 24) | ((unsigned char)bufferInt[2] << 16) | ((unsigned char)bufferInt[1] << 8) | (unsigned char)bufferInt[0]);
		fileStream.read(bufferInt, 4);
		bitmapHeight = (int)(((unsigned char)bufferInt[3] << 24) | ((unsigned char)bufferInt[2] << 16) | ((unsigned char)bufferInt[1] << 8) | (unsigned char)bufferInt[0]);

		fileStream.ignore(2);
		char colorDepthBuffer[2];
		fileStream.read(colorDepthBuffer, 2);
		colorDepth = (short)(((unsigned char)colorDepthBuffer[1] << 8) | ((unsigned char)colorDepthBuffer[0]));
		break;
	}

	fileStream.close();
}