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
	// Load the config
	std::string config_path;
	std::map<std::string, std::map<std::string, std::string>> config;
	std::map<std::string, std::map<std::string, std::string>> config_usr;

	bool cfg_sys = std::filesystem::exists("/usr/share/sys64/power/config.conf");
	bool cfg_sys_local = std::filesystem::exists("/usr/local/share/sys64/power/config.conf");
	bool cfg_usr = std::filesystem::exists(std::string(getenv("HOME")) + "/.config/sys64/power/config.conf");

	// Load default config
	if (cfg_sys)
		config_path = "/usr/share/sys64/power/config.conf";
	else if (cfg_sys_local)
		config_path = "/usr/local/share/sys64/power/config.conf";
	else
		std::fprintf(stderr, "No default config found, Things will get funky!\n");

	config = config_parser(config_path).data;

	// Load user config
	if (cfg_usr)
		config_path = std::string(getenv("HOME")) + "/.config/sys64/power/config.conf";
	else
		std::fprintf(stderr, "No user config found\n");

	config_usr = config_parser(config_path).data;

	// Merge configs
	for (const auto& [key, nested_map] : config_usr)
		for (const auto& [inner_key, inner_value] : nested_map)
			config[key][inner_key] = inner_value;

	// Sanity check
	if (!(cfg_sys || cfg_sys_local || cfg_usr)) {
		std::fprintf(stderr, "No config available, Something ain't right here.");
		return 1;
	}
	#endif

	// Read launch arguments
	#ifdef CONFIG_RUNTIME
	while (true) {
		switch(getopt(argc, argv, "p:m:t:vh")) {
			case 'p':
				config["main"]["position"] = optarg;
				continue;

			case 'm':
				config["main"]["monitor"] = optarg;
				continue;

			case 't':
				config["main"]["transition-duration"] = optarg;
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
	syspower *window = syspower_create_ptr(config);

	// Add window
	app->signal_startup().connect([&]() {
		app->add_window(*window);
	});

	return app->run();
}
