#pragma once

namespace pcb {
	enum class PixelDataFormat { RGB, BGR };

	class Image final {
	public:
		Image(char const* sourcePixels, int pixelArraySize, int width, int height, PixelDataFormat pixelDataFormat);
		~Image();

		int getWidth() const;
		int getHeight() const;
		char* getPixels() const;	// This doesn't add anything compared to having a public field?
		PixelDataFormat getPixelDataFormat() const;

	private:
		char* pixelArray;
		int widthInPixels;
		int heightInPixels;
		PixelDataFormat pixelDataFormat;
	};
}