#pragma once

#include <string>

class Image {
public:
	Image();
	~Image();
};

class BitmapLoader {
public:
	Image* loadFromFile(std::string filename);
};