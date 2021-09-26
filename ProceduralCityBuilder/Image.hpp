#pragma once

#include <vector>

namespace pcb {
	enum class PixelDataFormat { RGB, BGR };

	class Image final {
	public:
		Image(const std::vector<char>& sourcePixels, int width, int height, PixelDataFormat pixelDataFormat);
		Image(char const* sourcePixels, int pixelArraySize, int width, int height, PixelDataFormat pixelDataFormat);

		int getWidth() const;
		int getHeight() const;
		std::vector<char> getPixels() const;
		PixelDataFormat getPixelDataFormat() const;

	private:
		std::vector<char> pixelValues;
		int widthInPixels;
		int heightInPixels;
		PixelDataFormat pixelDataFormat;
	};
}