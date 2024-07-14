#include "main.hpp"
#include "config.hpp"
#include "git_info.hpp"

#include <gtkmm/application.h>
#include <iostream>
#include <dlfcn.h>

void load_libsyspower() {
	void* handle = dlopen("libsyspower.so", RTLD_LAZY);
	if (!handle) {
		std::cerr << "Cannot open library: " << dlerror() << '\n';
		exit(1);
	}

	syspower_create_ptr = (syspower_create_func)dlsym(handle, "syspower_create");
	syspower_show_windows_ptr = (syspower_show_windows_func)dlsym(handle, "syspower_show_windows");

	if (!syspower_create_ptr || !syspower_show_windows_ptr) {
		std::cerr << "Cannot load symbols: " << dlerror() << '\n';
		dlclose(handle);
		exit(1);
	}
}

int main(int argc, char *argv[]) {

	// Read launch arguments
	#ifdef RUNTIME_CONFIG
	while (true) {
		switch(getopt(argc, argv, "p:dm:dt:dvh")) {
			case 'p':
				config_main.position = std::stoi(optarg);
				if (config_main.position > 4 || config_main.position < 0) {
					std::cerr << "Invalid position value" << std::endl;
					return 1;
				}
				continue;

			case 'm':
				config_main.main_monitor = std::stoi(optarg);
				if (config_main.main_monitor < 0) {
					std::cerr << "Invalid primary monitor value" << std::endl;
					return 1;
				}
				continue;

			case 't':
				config_main.transition_duration = std::stoi(optarg);
				if (config_main.transition_duration < 0 || config_main.position < 0) {
					std::cerr << "Invalid transition duration value" << std::endl;
					return 1;
				}
				continue;

			case 'v':
				std::cout << "Commit: " << GIT_COMMIT_MESSAGE << std::endl;
				std::cout << "Date: " << GIT_COMMIT_DATE << std::endl;
				return 0;

			case 'h':
			default :
				std::cout << "usage:" << std::endl;
				std::cout << "  syspower [argument...]:\n" << std::endl;
				std::cout << "arguments:" << std::endl;
				std::cout << "  -p	Set position" << std::endl;
				std::cout << "  -m	Set primary monitor" << std::endl;
				std::cout << "  -t	Set revealer transition duration" << std::endl;
				std::cout << "  -v	Prints version info" << std::endl;
				std::cout << "  -h	Show this help message" << std::endl;
				return 0;

			case -1:
				break;
			}

			break;
	}
	#endif

	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create("funky.sys64.syspower");

	load_libsyspower();
	syspower *window = syspower_create_ptr(config_main);

	// Add windows
	app->signal_startup().connect([&]() {
		app->add_window(*window);
		syspower_show_windows_ptr(window);
	});

	return app->run();
}
