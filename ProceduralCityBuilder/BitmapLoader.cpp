#include <fstream>

#include "BitmapLoader.hpp"
#include "BitshiftingUtility.hpp"
#include <cmath>

pcb::Image* pcb::BitmapLoader::loadFromFile(std::string filename) {
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
	int pixelArrayOffset = pcb::littleToBigEndian<int>(bufferInt, 4);

	fileStream.read(bufferInt, 4);
	int headerSize = pcb::littleToBigEndian<unsigned int>(bufferInt, 4);

	int bitmapWidth = 0;
	int bitmapHeight = 0;	// TODO: If negative, pixels stored from top to bottom. Else (default) stored from bottom to top.
	short colorDepth = 0;

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
		bitmapWidth = pcb::littleToBigEndian<int>(bufferInt, 4);
		fileStream.read(bufferInt, 4);
		bitmapHeight = pcb::littleToBigEndian<int>(bufferInt, 4);

		fileStream.ignore(2);
		char colorDepthBuffer[2];
		fileStream.read(colorDepthBuffer, 2);
		colorDepth = pcb::littleToBigEndian<short>(colorDepthBuffer, 2);
		break;
	}

	int pixelStorageRowSizeInBytes = static_cast<int>(4 * std::ceil((colorDepth * bitmapWidth) / 32.0f));
	int pixelArraySizeInBytes = pixelStorageRowSizeInBytes * std::abs(bitmapHeight);
	char* pixelBuffer = new char[pixelArraySizeInBytes];
	fileStream.seekg(pixelArrayOffset);
	fileStream.read(pixelBuffer, pixelArraySizeInBytes);

	fileStream.close();

	char* pixelBufferReOrdered = new char[pixelArraySizeInBytes];


	Image* image = new Image(pixelBuffer, pixelArraySizeInBytes, bitmapWidth, bitmapHeight, pcb::PixelDataFormat::BGR);		// TODO: Pixel data format should be read from file, not hardcoded like this.
	delete[] pixelBuffer;
	delete[] pixelBufferReOrdered;

	return image;
}