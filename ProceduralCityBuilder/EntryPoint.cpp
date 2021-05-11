#include "Application.hpp"
#include "BitmapLoader.hpp"

int main(int argc, char* argv[]) {
	BitmapLoader loader;
	loader.loadFromFile("test.bmp");

	Application application;
	application.run(&application, argc, argv);

	return 0;
}