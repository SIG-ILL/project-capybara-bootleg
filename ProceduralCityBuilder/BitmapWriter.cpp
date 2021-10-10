#include "BitmapWriter.hpp"

#include <fstream>

#include "BitshiftingUtility.hpp"

void pcb::BitmapWriter::writeToFile(const Image& image, const std::string& filepath) const {
	std::ofstream fileStream;
	fileStream.open(filepath, std::fstream::binary);
	if (!fileStream.is_open()) {
		return;
	}

	/*
	* BM (2 bytes)
	* 4 bytes size of file in bytes
	* 4 bytes ignore
	* 4 bytes pixel array offset
	* 
	* 4 bytes header size
	* [with header size 40]:
	* 4 bytes image width
	* 4 bytes image height
	* 2 bytes number of color planes (must be 1)
	* 2 bytes color depth
	* 4 bytes compression method
	* 4 bytes image size (raw bitmap data including padding, 0 can be given if compression method == 0 (BI_RGB))
	* 4 bytes horizontal resolution (pixel per metre, signed integer)
	* 4 bytes vertical resolution (pixel per metre, signed integer)
	* 4 bytes number of colors in color palette (0 to default to 2^n)
	* 4 bytes number of important colors used (0 when every color is important, generally ignored)
	* 
	* pixel array at offset indicated earlier (rows are multiples of 4 bytes, add padding if needed)
	*/

	const int FILE_HEADER_SIZE = 14;
	const int BITMAP_INFO_HEADER_SIZE = 40;
	const int TOTAL_HEADERS_SIZE = FILE_HEADER_SIZE + BITMAP_INFO_HEADER_SIZE;
	const int BITS_PER_BYTE = 8;
	std::vector<unsigned char> pixelArrayOffset = pcb::bigToLittleEndian<int, 4>(TOTAL_HEADERS_SIZE);
	std::vector<unsigned char> headerSize = pcb::bigToLittleEndian<int, 4>(40);
	std::vector<unsigned char> imageWidth = pcb::bigToLittleEndian<int, 4>(image.getWidth());
	std::vector<unsigned char> imageHeight = pcb::bigToLittleEndian<int, 4>(image.getHeight());
	std::vector<unsigned char> colorDepth = pcb::bigToLittleEndian<short, 2>(3 * BITS_PER_BYTE);
	std::vector<unsigned char> colorPlanes = pcb::bigToLittleEndian<short, 2>(1);
	std::vector<unsigned char> compressionMethod = pcb::bigToLittleEndian<int, 4>(0);
	std::vector<unsigned char> rawImageSize = pcb::bigToLittleEndian<int, 4>(0);
	std::vector<unsigned char> printResolution = pcb::bigToLittleEndian<int, 4>(2835);
	std::vector<unsigned char> colorsInPalette = pcb::bigToLittleEndian<int, 4>(0);
	std::vector<unsigned char> importantColors = pcb::bigToLittleEndian<int, 4>(0);
	std::shared_ptr<std::vector<char>> pixelArray = image.getPixels();
	
	int fileSize = pixelArray->size() + TOTAL_HEADERS_SIZE;		// TODO: Include padding in size. Also actually apply padding when writing pixel array.
	std::vector<unsigned char> fileSizeConverted = pcb::bigToLittleEndian<int, 4>(fileSize);

	fileStream << "BM"
		<< fileSizeConverted[0] << fileSizeConverted[1] << fileSizeConverted[2] << fileSizeConverted[3]
		<< '\0' << '\0'
		<< '\0' << '\0'
		<< pixelArrayOffset[0] << pixelArrayOffset[1] << pixelArrayOffset[2] << pixelArrayOffset[3]

		<< headerSize[0] << headerSize[1] << headerSize[2] << headerSize[3]
		<< imageWidth[0] << imageWidth[1] << imageWidth[2] << imageWidth[3]
		<< imageHeight[0] << imageHeight[1] << imageHeight[2] << imageHeight[3]
		<< colorPlanes[0] << colorPlanes[1]
		<< colorDepth[0] << colorDepth[1]
		<< compressionMethod[0] << compressionMethod[1] << compressionMethod[2] << compressionMethod[3]
		<< rawImageSize[0] << rawImageSize[1] << rawImageSize[2] << rawImageSize[3]
		<< printResolution[0] << printResolution[1] << printResolution[2] << printResolution[3]
		<< printResolution[0] << printResolution[1] << printResolution[2] << printResolution[3]
		<< colorsInPalette[0] << colorsInPalette[1] << colorsInPalette[2] << colorsInPalette[3]
		<< importantColors[0] << importantColors[1] << importantColors[2] << importantColors[3];
	
	// TODO: Apply padding when necessary.
	for (int i = 0; i < pixelArray->size(); i++) {
		fileStream << (*pixelArray)[i];
	}

	fileStream.close();
}