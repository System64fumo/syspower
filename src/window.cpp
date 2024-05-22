#include "window.hpp"
#include "main.hpp"
#include "config.hpp"

#include <thread>
#include <iostream>
#include <cstdlib>
#include <string>
#include <iomanip>
#include <gtk4-layer-shell.h>
#include <gtkmm/cssprovider.h>
#include <filesystem>

std::string action_type;

void thread() {
	char command[30] = "";

	// Figure out what we're doing
	if (action_type == "shutdown") {
		if (access("/bin/systemctl", F_OK) != -1)
			// Systemd-logind
			strcpy(command, "systemctl poweroff");
		else
			// Elogind
			strcpy(command, "loginctl poweroff");

		win->label_status.set_label("Shutting down...");
	}
	else if (action_type == "reboot") {
		if (access("/bin/systemctl", F_OK) != -1)
			// Systemd-logind
			strcpy(command, "systemctl reboot");
		else
			// Elogind
			strcpy(command, "loginctl reboot");

		win->label_status.set_label("Rebooting...");
	}
	else if (action_type == "logout") {
		strcpy(command, "loginctl terminate-user $USER");
		win->label_status.set_label("Logging out...");
	}

	// Revealer
	if (transition_duration != 0) {
		win->revealer_box.set_reveal_child(false);
		usleep(transition_duration * 1000);
		win->revealer_box.set_visible(false);
	}
	else
		win->box_buttons.set_visible(false);

	// Set proper layout
	win->box_layout.set_valign(Gtk::Align::CENTER);
	win->box_layout.set_halign(Gtk::Align::CENTER);

	win->label_status.set_visible(true);
	win->progressbar_sync.set_visible(true);
	win->max_slider_value = get_dirty_pages();

	// TODO: Add a dynamic class based on progress
	// Will be useful for custom css where the screen gets dimmer
	// based on progress.

	kill_child_processes();

	// Sync filesystems
	win->timer_connection = Glib::signal_timeout().connect(sigc::mem_fun(*win, &syspower::on_timer_tick), 50);
	win->progressbar_sync.set_fraction(1);
	sync_filesystems();

	// Cleanup
	win->progressbar_sync.set_fraction(0);
	win->timer_connection.disconnect();

	// Run action
	system(command);
}

void syspower::show_other_windows() {
	// Get all monitors
	display = gdk_display_get_default();
	monitors = gdk_display_get_monitors(display);
	gtk_layer_set_monitor (gobj(), GDK_MONITOR(g_list_model_get_item(monitors, main_monitor)));

	int monitorCount = g_list_model_get_n_items(monitors);

	if (main_monitor >= monitorCount) {
		std::cerr << "Invalid primary monitor value" << std::endl;
		app->quit();
	}

	for (int i = 0; i < monitorCount; ++i) {
		// Ignore primary monitor
		if (i == main_monitor)
			continue;

		GdkMonitor *monitor = GDK_MONITOR(g_list_model_get_item(monitors, i));

		// Create empty windows
		Gtk::Window *window = new Gtk::Window();
		app->add_window(*window);

		// Layer shell stuff
		gtk_layer_init_for_window(window->gobj());
		gtk_layer_set_namespace(gobj(), "syspower_empty_window");
		gtk_layer_set_layer(window->gobj(), GTK_LAYER_SHELL_LAYER_TOP);
		gtk_layer_set_anchor(window->gobj(), GTK_LAYER_SHELL_EDGE_LEFT, true);
		gtk_layer_set_anchor(window->gobj(), GTK_LAYER_SHELL_EDGE_RIGHT, true);
		gtk_layer_set_anchor(window->gobj(), GTK_LAYER_SHELL_EDGE_TOP, true);
		gtk_layer_set_anchor(window->gobj(), GTK_LAYER_SHELL_EDGE_BOTTOM, true);
		gtk_layer_set_monitor(window->gobj(), monitor);

		window->show();
	}
}

syspower::syspower() {
	// Layer shell stuff
	gtk_layer_init_for_window(gobj());
	gtk_layer_set_keyboard_mode(gobj(), GTK_LAYER_SHELL_KEYBOARD_MODE_ON_DEMAND);
	gtk_layer_set_namespace(gobj(), "syspower");
	gtk_layer_set_layer(gobj(), GTK_LAYER_SHELL_LAYER_TOP);

	// TODO: Add customization, For now assume fullscreen.
	gtk_layer_set_anchor(gobj(), GTK_LAYER_SHELL_EDGE_LEFT, true);
	gtk_layer_set_anchor(gobj(), GTK_LAYER_SHELL_EDGE_RIGHT, true);
	gtk_layer_set_anchor(gobj(), GTK_LAYER_SHELL_EDGE_TOP, true);
	gtk_layer_set_anchor(gobj(), GTK_LAYER_SHELL_EDGE_BOTTOM, true);

	// Initialization
	set_title("Power Menu");
	add_css_class("primary_window");
	set_default_size(200, 100);
	set_child(box_layout);
	show();

	box_layout.get_style_context()->add_class("box_layout");
	box_layout.property_orientation().set_value(Gtk::Orientation::VERTICAL);

	Gtk::RevealerTransitionType transition_type = Gtk::RevealerTransitionType::SLIDE_UP;

	switch (position) {
		case 0: // Top
			box_layout.set_valign(Gtk::Align::START);
			box_layout.set_halign(Gtk::Align::CENTER);
			transition_type = Gtk::RevealerTransitionType::SLIDE_DOWN;
			break;
		case 1: // Right
			box_layout.set_valign(Gtk::Align::CENTER);
			box_layout.set_halign(Gtk::Align::END);
			box_buttons.property_orientation().set_value(Gtk::Orientation::VERTICAL);
			transition_type = Gtk::RevealerTransitionType::SLIDE_LEFT;
			break;
		case 2: // Bottom
			box_layout.set_valign(Gtk::Align::END);
			box_layout.set_halign(Gtk::Align::CENTER);
			transition_type = Gtk::RevealerTransitionType::SLIDE_UP;
			break;
		case 3: // Left
			box_layout.set_valign(Gtk::Align::CENTER);
			box_layout.set_halign(Gtk::Align::START);
			box_buttons.property_orientation().set_value(Gtk::Orientation::VERTICAL);
			transition_type = Gtk::RevealerTransitionType::SLIDE_RIGHT;
			break;
		case 4: // Centered
			box_layout.set_valign(Gtk::Align::CENTER);
			box_layout.set_halign(Gtk::Align::CENTER);
			box_buttons.property_orientation().set_value(Gtk::Orientation::VERTICAL);
			transition_type = Gtk::RevealerTransitionType::SLIDE_UP;
			break;
	}

	box_buttons.get_style_context()->add_class("box_buttons");
	box_buttons.set_halign(Gtk::Align::CENTER);

	box_layout.append(label_status);

	// Revealer
	if (transition_duration != 0) {
		box_layout.append(revealer_box);
		revealer_box.set_child(box_buttons);
		revealer_box.set_transition_type(transition_type);
		revealer_box.set_transition_duration(0);
		revealer_box.set_reveal_child(true);
		revealer_box.set_transition_duration(transition_duration);
	}
	else
		box_layout.append(box_buttons);
	box_layout.append(progressbar_sync);

	label_status.get_style_context()->add_class("label_status");
	label_status.set_margin(10);
	label_status.set_visible(false);
	progressbar_sync.get_style_context()->add_class("progressbar_sync");
	progressbar_sync.set_size_request(300, -1);
	progressbar_sync.set_visible(false);

	// Button shenanigans
	box_buttons.append(button_shutdown);
	button_shutdown.get_style_context()->add_class("button_shutdown");
	button_shutdown.set_size_request(300, 75);
	button_shutdown.set_label("Shutdown");
	button_shutdown.set_margin(5);

	box_buttons.append(button_reboot);
	button_reboot.get_style_context()->add_class("button_reboot");
	button_reboot.set_size_request(300, 75);
	button_reboot.set_label("Reboot");
	button_reboot.set_margin(5);

	box_buttons.append(button_logout);
	button_logout.get_style_context()->add_class("button_logout");
	button_logout.set_size_request(300, 75);
	button_logout.set_label("Logout");
	button_logout.set_margin(5);

	box_buttons.append(button_cancel);
	button_cancel.get_style_context()->add_class("button_cancel");
	button_cancel.set_size_request(300, 75);
	button_cancel.set_label("Cancel");
	button_cancel.set_margin(5);

	// Signals
	button_shutdown.signal_clicked().connect(sigc::mem_fun(*this, &syspower::button_shutdown_clicked));
	button_reboot.signal_clicked().connect(sigc::mem_fun(*this, &syspower::button_reboot_clicked));
	button_logout.signal_clicked().connect(sigc::mem_fun(*this, &syspower::button_logout_clicked));
	button_cancel.signal_clicked().connect(sigc::mem_fun(*this, &syspower::button_cancel_clicked));

	// Load custom css
	std::string home_dir = getenv("HOME");
	std::string css_path = home_dir + "/.config/sys64/power.css";

	if (!std::filesystem::exists(css_path)) return;

	auto css = Gtk::CssProvider::create();
	css->load_from_path(css_path);
	auto style_context = get_style_context();
	style_context->add_provider_for_display(property_display(), css, GTK_STYLE_PROVIDER_PRIORITY_USER);
}

// TODO: Fix this horrible mess, learn to use dynamic casting or something idk.
void syspower::button_shutdown_clicked() {
	action_type = "shutdown";
	thread_action = std::thread(thread);
}

void syspower::button_reboot_clicked() {
	action_type = "reboot";
	thread_action = std::thread(thread);
}

void syspower::button_logout_clicked() {
	action_type = "logout";
	thread_action = std::thread(thread);
}

void syspower::button_cancel_clicked() {
	app->quit();
}

bool syspower::on_timer_tick() {
	double dirty_pages_kb = get_dirty_pages();
	double value = dirty_pages_kb / max_slider_value;

	if (progressbar_sync.get_fraction() > value)
		progressbar_sync.set_fraction(value);

	return true;
}
