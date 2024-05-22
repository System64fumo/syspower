#include "window.hpp"
#include "main.hpp"
#include "config.hpp"

#include <iostream>
#include <getopt.h>

int main(int argc, char *argv[]) {

	// Read launch arguments
	while (true) {
		switch(getopt(argc, argv, "p:dm:dh")) {
			case 'p':
				position = std::stoi(optarg);
				continue;

			case 'm':
				main_monitor = std::stoi(optarg);
				continue;

			case 'h':
			default :
				std::cout << "usage:" << std::endl;
				std::cout << "  syspower [argument...]:\n" << std::endl;
				std::cout << "arguments:" << std::endl;
				std::cout << "  -p	Set position" << std::endl;
				std::cout << "  -m	Set primary monitor" << std::endl;
				std::cout << "  -h	Show this help message" << std::endl;
				return 0;

			case -1:
				break;
			}

			break;
	}

	app = Gtk::Application::create("funky.sys64.syspower");
	win = new syspower();
	app->hold();

	// Other monitors
	app->signal_startup().connect([&]() {
		win->show_other_windows();
	});

	app->run();

	return 0;
}
