#include "window.hpp"
#include "main.hpp"

int main(int argc, char *argv[]) {
	// TODO: Add ability to read arguments later.
	// TODO: Add empty window to other screens if available.
	app = Gtk::Application::create("funky.sys64.syspower");
	win = new syspower();
	app->hold();
	app->run();

	return 0;
}
