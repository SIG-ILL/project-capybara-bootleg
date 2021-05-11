#include "Application.hpp"

int main(int argc, char* argv[]) {
	pcb::Application application;
	application.run(&application, argc, argv);

	return 0;
}