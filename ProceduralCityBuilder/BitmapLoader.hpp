#pragma once

#include <string>

namespace pcb {
	class Image {
	public:
		Image();
		Image(char* sourcePixels, int pixelArraySize, int width, int height);
		~Image();

		int getWidth();
		int getHeight();
		char* getPixels();	// This doesn't add anything compared to having a public field?

	private:
		char* pixelArray;
		int widthInPixels;
		int heightInPixels;
	};

	class BitmapLoader {
	public:
		Image* loadFromFile(std::string filename);
	};
}