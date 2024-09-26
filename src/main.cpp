#include "main.hpp"
#include "config.hpp"
#include "config_parser.hpp"
#include "git_info.hpp"

#include <gtkmm/application.h>
#include <filesystem>
#include <dlfcn.h>

void load_libsyspower() {
	void* handle = dlopen("libsyspower.so", RTLD_LAZY);
	if (!handle) {
		std::fprintf(stderr,"Cannot open library: %s\n", dlerror());
		exit(1);
	}

	syspower_create_ptr = (syspower_create_func)dlsym(handle, "syspower_create");

	if (!syspower_create_ptr) {
		std::fprintf(stderr,"Cannot load symbols: %s\n", dlerror());
		dlclose(handle);
		exit(1);
	}
}

int main(int argc, char *argv[]) {
	// Load the config
	#ifdef CONFIG_FILE
	std::string config_path;
	if (std::filesystem::exists(std::string(getenv("HOME")) + "/.config/sys64/power/config.conf"))
		config_path = std::string(getenv("HOME")) + "/.config/sys64/power/config.conf";
	else if (std::filesystem::exists("/usr/share/sys64/power/config.conf"))
		config_path = "/usr/share/sys64/power/config.conf";
	else
		config_path = "/usr/local/share/sys64/power/config.conf";

	config_parser config(config_path);

	if (config.available) {
		std::string cfg_position = config.data["main"]["position"];
		if (!cfg_position.empty())
			config_main.position = std::stoi(cfg_position);

		std::string cfg_monitor =  config.data["main"]["monitor"];
		if (!cfg_monitor.empty())
			config_main.main_monitor=std::stoi(cfg_monitor);

		std::string cfg_transition =  config.data["main"]["transition-duration"];
		if (!cfg_transition.empty())
			config_main.transition_duration =std::stoi(cfg_transition);
	}
	#endif

	// Read launch arguments
	#ifdef CONFIG_RUNTIME
	while (true) {
		switch(getopt(argc, argv, "p:dm:dt:dvh")) {
			case 'p':
				config_main.position = std::stoi(optarg);
				if (config_main.position > 4 || config_main.position < 0) {
					std::fprintf(stderr,"Invalid position value\n");
					return 1;
				}
				continue;

			case 'm':
				config_main.main_monitor = std::stoi(optarg);
				if (config_main.main_monitor < 0) {
					std::fprintf(stderr,"Invalid primary monitor value\n");
					return 1;
				}
				continue;

			case 't':
				config_main.transition_duration = std::stoi(optarg);
				if (config_main.transition_duration < 0 || config_main.position < 0) {
					std::fprintf(stderr,"Invalid transition duration value\n");
					return 1;
				}
				continue;

			case 'v':
				std::printf("Commit: %s\n", GIT_COMMIT_MESSAGE);
				std::printf("Date: %s\n", GIT_COMMIT_DATE);
				return 0;

			case 'h':
			default :
				std::printf("usage:\n");
				std::printf("  syspower [argument...]:\n\n");
				std::printf("arguments:\n");
				std::printf("  -p	Set position\n");
				std::printf("  -m	Set primary monitor\n");
				std::printf("  -t	Set revealer transition duration\n");
				std::printf("  -v	Prints version info\n");
				std::printf("  -h	Show this help message\n");
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

	// Add window
	app->signal_startup().connect([&]() {
		app->add_window(*window);
	});

	return app->run();
}
