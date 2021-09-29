#include "Application.hpp"

#include <memory>

int main(int argc, char* argv[]) {
	std::shared_ptr<pcb::Application> application = std::make_shared<pcb::Application>();
	application->run(application, argc, argv);

	return 0;
}