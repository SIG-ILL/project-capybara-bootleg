#include "Image.hpp"

pcb::Image::Image(std::shared_ptr<std::vector<char>> sourcePixels, int width, int height, PixelDataFormat pixelDataFormat) : pixelValues(sourcePixels), widthInPixels(width), heightInPixels(height), pixelDataFormat(pixelDataFormat) {}

int pcb::Image::getWidth() const { return widthInPixels; }

int pcb::Image::getHeight() const { return heightInPixels; }

std::shared_ptr<std::vector<char>> pcb::Image::getPixels() const { return pixelValues; }

pcb::PixelDataFormat pcb::Image::getPixelDataFormat() const { return pixelDataFormat; }