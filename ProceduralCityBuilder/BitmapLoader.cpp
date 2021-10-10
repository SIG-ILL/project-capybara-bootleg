#include "BitmapLoader.hpp"

#include <fstream>
#include <cmath>
#include <array>

#include "BitshiftingUtility.hpp"


std::unique_ptr<pcb::Image> pcb::BitmapLoader::loadFromFile(const std::string& filepath) const {
	std::ifstream fileStream;
	fileStream.open(filepath, std::fstream::binary);
	if (!fileStream.is_open()) {
		return nullptr;
	}

	std::array<char, 2> bufferBitMapHeader;
	fileStream.read(bufferBitMapHeader.data(), 2);
	if (bufferBitMapHeader[0] != 'B' || bufferBitMapHeader[1] != 'M') {
		fileStream.close();
		return nullptr;
	}

	fileStream.ignore(8);

	std::array<char, 4> bufferInt;
	fileStream.read(bufferInt.data(), 4);
	int pixelArrayOffset = pcb::littleToBigEndian<int, 4>(bufferInt);

	fileStream.read(bufferInt.data(), 4);
	int headerSize = pcb::littleToBigEndian<unsigned int, 4>(bufferInt);

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
		fileStream.read(bufferInt.data(), 4);
		bitmapWidth = pcb::littleToBigEndian<int, 4>(bufferInt);
		fileStream.read(bufferInt.data(), 4);
		bitmapHeight = pcb::littleToBigEndian<int, 4>(bufferInt);

		fileStream.ignore(2);
		std::array<char, 2> colorDepthBuffer;
		fileStream.read(colorDepthBuffer.data(), 2);
		colorDepth = pcb::littleToBigEndian<short, 2>(colorDepthBuffer);
		break;
	}

	int pixelStorageRowSizeInBytes = static_cast<int>(4 * std::ceil((colorDepth * bitmapWidth) / 32.0f));
	int pixelArraySizeInBytes = pixelStorageRowSizeInBytes * std::abs(bitmapHeight);
	char* pixelBuffer = new char[pixelArraySizeInBytes];
	fileStream.seekg(pixelArrayOffset);
	fileStream.read(pixelBuffer, pixelArraySizeInBytes);
	fileStream.close();

	std::vector<char> pixelBufferReOrdered(pixelArraySizeInBytes);

	std::unique_ptr<std::vector<char>> pixelBufferVector = std::make_unique<std::vector<char>>(pixelArraySizeInBytes);
	for (int i = 0; i < pixelArraySizeInBytes; i++) {
		(*pixelBufferVector)[i] = pixelBuffer[i];
	}

	std::unique_ptr<Image> image = std::make_unique<Image>(std::move(pixelBufferVector), bitmapWidth, bitmapHeight, pcb::PixelDataFormat::BGR);	// TODO: Pixel data format should be read from file, not hardcoded like this.
	delete[] pixelBuffer;

	return image;
}