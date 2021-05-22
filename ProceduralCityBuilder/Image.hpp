#pragma once

namespace pcb {
	enum class PixelDataFormat { RGB, BGR };

	class Image final {
	public:
		Image(char const* sourcePixels, int pixelArraySize, int width, int height, PixelDataFormat pixelDataFormat);
		~Image();

		int getWidth();
		int getHeight();
		char* getPixels();	// This doesn't add anything compared to having a public field?
		PixelDataFormat getPixelDataFormat();

	private:
		char* pixelArray;
		int widthInPixels;
		int heightInPixels;
		PixelDataFormat pixelDataFormat;
	};
}