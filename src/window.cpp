#include "window.hpp"
#include "functions.hpp"

#include <gtk4-layer-shell.h>
#include <gtkmm/cssprovider.h>
#include <gtkmm/eventcontrollerkey.h>
#include <glibmm/main.h>
#include <filesystem>
#include <thread>

syspower::syspower(const std::map<std::string, std::map<std::string, std::string>>& cfg) : config_main(cfg) {
	config_main = cfg;

	// Layer shell stuff
	gtk_layer_init_for_window(gobj());
	gtk_layer_set_keyboard_mode(gobj(), GTK_LAYER_SHELL_KEYBOARD_MODE_ON_DEMAND);
	gtk_layer_set_namespace(gobj(), "syspower");
	gtk_layer_set_layer(gobj(), GTK_LAYER_SHELL_LAYER_OVERLAY);

	// TODO: Add customization, For now assume fullscreen.
	gtk_layer_set_anchor(gobj(), GTK_LAYER_SHELL_EDGE_LEFT, true);
	gtk_layer_set_anchor(gobj(), GTK_LAYER_SHELL_EDGE_RIGHT, true);
	gtk_layer_set_anchor(gobj(), GTK_LAYER_SHELL_EDGE_TOP, true);
	gtk_layer_set_anchor(gobj(), GTK_LAYER_SHELL_EDGE_BOTTOM, true);

	// Initialization
	set_name("syspower");
	set_title("Power Menu");
	add_css_class("primary_window");
	set_default_size(200, 100);
	set_child(box_layout);
	show_other_windows();

	box_layout.get_style_context()->add_class("box_layout");
	box_layout.property_orientation().set_value(Gtk::Orientation::VERTICAL);

	Gtk::RevealerTransitionType transition_type = Gtk::RevealerTransitionType::SLIDE_UP;

	switch (std::stoi(config_main["main"]["position"])) {
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
	if (std::stoi(config_main["main"]["transition-duration"]) != 0) {
		box_layout.append(revealer_box);
		revealer_box.set_child(box_buttons);
		revealer_box.set_transition_type(transition_type);
		revealer_box.set_transition_duration(0);
		revealer_box.set_reveal_child(true);
		revealer_box.set_transition_duration(std::stoi(config_main["main"]["transition-duration"]));
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
	add_button("Shutdown");
	add_button("Reboot");
	add_button("Logout");
	add_button("Suspend");
	add_button("Hibernate");
	add_button("Cancel");

	// Key events
	auto key_controller = Gtk::EventControllerKey::create();
	key_controller->signal_key_pressed().connect(
		[this](guint keyval, guint, Gdk::ModifierType) {
			std::string unicode_char = std::string(1, static_cast<char>(gdk_keyval_to_unicode(keyval)));
			// TODO: This is temporary until custom button support is added
			if (!config_main["hotkeys"][unicode_char].empty()) {
				on_button_clicked(config_main["hotkeys"][unicode_char]);
				return true;
			}
			// TODO: Somehow reuse code above for this in the laziest way possible
			else if (keyval == 0xff1b) {
				on_button_clicked("cancel");
				return true;
			}
			return false;
		},
		false
	);
	add_controller(key_controller);

	const std::string& style_path = "/usr/share/sys64/power/style.css";
	const std::string& style_path_usr = std::string(getenv("HOME")) + "/.config/sys64/power/style.css";

	// Load base style
	if (std::filesystem::exists(style_path)) {
		auto css = Gtk::CssProvider::create();
		css->load_from_path(style_path);
		get_style_context()->add_provider_for_display(property_display(), css, GTK_STYLE_PROVIDER_PRIORITY_USER);
	}
	// Load user style
	if (std::filesystem::exists(style_path_usr)) {
		auto css = Gtk::CssProvider::create();
		css->load_from_path(style_path_usr);
		get_style_context()->add_provider_for_display(property_display(), css, GTK_STYLE_PROVIDER_PRIORITY_USER);
	}
}

void syspower::show_other_windows() {
	// Get all monitors
	display = gdk_display_get_default();
	monitors = gdk_display_get_monitors(display);
	gtk_layer_set_monitor(gobj(), GDK_MONITOR(g_list_model_get_item(monitors, std::stoi(config_main["main"]["monitor"]))));
	show();

	int monitorCount = g_list_model_get_n_items(monitors);

	if (std::stoi(config_main["main"]["monitor"]) >= monitorCount)
		config_main["main"]["monitor"] = std::to_string(monitorCount - 1);

	for (int i = 0; i < monitorCount; ++i) {
		// Ignore primary monitor
		if (i == std::stoi(config_main["main"]["monitor"]))
			continue;

		GdkMonitor *monitor = GDK_MONITOR(g_list_model_get_item(monitors, i));

		// Create empty windows
		auto window = std::make_shared<Gtk::Window>();
		window->set_name("syspower");
		window->add_css_class("empty_window");

		// Layer shell stuff
		gtk_layer_init_for_window(window->gobj());
		gtk_layer_set_namespace(window->gobj(), "syspower-empty-window");
		gtk_layer_set_layer(window->gobj(), GTK_LAYER_SHELL_LAYER_TOP);
		gtk_layer_set_anchor(window->gobj(), GTK_LAYER_SHELL_EDGE_LEFT, true);
		gtk_layer_set_anchor(window->gobj(), GTK_LAYER_SHELL_EDGE_RIGHT, true);
		gtk_layer_set_anchor(window->gobj(), GTK_LAYER_SHELL_EDGE_TOP, true);
		gtk_layer_set_anchor(window->gobj(), GTK_LAYER_SHELL_EDGE_BOTTOM, true);
		gtk_layer_set_monitor(window->gobj(), monitor);

		windows.push_back(window);
		window->show();
	}
}

void syspower::action_thread() {
	// Revealer
	if (std::stoi(config_main["main"]["transition-duration"]) != 0) {
		revealer_box.set_reveal_child(false);
		usleep(std::stoi(config_main["main"]["transition-duration"]) * 1000);
		revealer_box.set_visible(false);
	}
	else
		box_buttons.set_visible(false);

	// Set proper layout
	box_layout.set_valign(Gtk::Align::CENTER);
	box_layout.set_halign(Gtk::Align::CENTER);

	label_status.set_visible(true);
	progressbar_sync.set_visible(true);
	max_slider_value = syspower_functions::get_dirty_pages();

	// TODO: Add a dynamic class based on progress
	// Will be useful for custom css where the screen gets dimmer
	// based on progress.
	label_status.set_label("Closing programs...");
	progressbar_sync.set_fraction(1);
	syspower_functions::kill_child_processes();

	// Sync filesystems
	label_status.set_label("Syncing filesystems...");
	timer_connection = Glib::signal_timeout().connect(sigc::mem_fun(*this, &syspower::on_timer_tick), 50);
	syspower_functions::sync_filesystems();

	// Cleanup
	progressbar_sync.set_fraction(0);
	progressbar_sync.set_visible(false);
	timer_connection.disconnect();

	// Run action
	usleep(100 * 1000);
	label_status.set_label(button_text);
	Glib::signal_idle().connect([this]() {
		sleep(15); // Timeout
		label_status.set_label("This seems to be taking longer than usual..");
		auto children = box_buttons.get_children();
		for (auto child : children)
			child->set_visible(false);

		children.back()->set_visible(true);
		revealer_box.set_visible(true);
		revealer_box.set_reveal_child(true);
		box_buttons.set_visible(true);
		return false;
	});
	int ret = system(command.c_str());
	(void)ret; // Unused variable
}

void syspower::add_button(const std::string& label) {
	Gtk::Button *button = Gtk::make_managed<Gtk::Button>(label);
	std::string lowercase = label;
	for (char& c : lowercase)
		c = std::tolower(static_cast<unsigned char>(c));
	button->set_size_request(300, 75);
	button->set_margin(5);
	button->get_style_context()->add_class("button_" + lowercase);
	box_buttons.append(*button);
	button->signal_clicked().connect([this, lowercase]() {
		on_button_clicked(lowercase);
	});
}

void syspower::on_button_clicked(const std::string& button) {
	std::string cmd = "systemctl";
	if (!systemd(cmd))
		cmd = "loginctl";

	command += cmd;

	// Figure out what we're doing
	if (button == "shutdown") {
		command +=  " poweroff";
		button_text = "Shutting down...";
	}
	else if (button == "reboot") {
		command += " reboot";
		button_text = "Rebooting...";
	}
	else if (button == "logout") {
		command += " terminate-user $USER";
		button_text = "Logging out...";
	}
	else if (button == "suspend") {
		button_text = "Suspending...";
		system((cmd + " suspend").c_str());
		for (const auto &window : windows)
			window->close();
		close();
	}
	else if (button == "hibernate") {
		button_text = "Hibernating...";
		system((cmd + " hibernate").c_str());
		for (const auto &window : windows)
			window->close();
		close();
	}
	else if (button == "cancel") {
		for (const auto &window : windows)
			window->close();
		close();
		return;
	}

	std::thread thread_action(&syspower::action_thread, this);
	thread_action.detach();
}

bool syspower::on_timer_tick() {
	double dirty_pages_kb = syspower_functions::get_dirty_pages();
	double value = dirty_pages_kb / max_slider_value;

	if (progressbar_sync.get_fraction() > value)
		progressbar_sync.set_fraction(value);

	return true;
}

bool syspower::systemd(const std::string& cmd) {
	std::string path_env_str(std::getenv("PATH"));
	std::vector<std::string> paths;
	size_t start = 0, end;

	while ((end = path_env_str.find(':', start)) != std::string::npos) {
		paths.emplace_back(path_env_str.substr(start, end - start));
		start = end + 1;
	}
	paths.emplace_back(path_env_str.substr(start));

	for (const auto& dir : paths) {
		if (std::filesystem::exists(std::filesystem::path(dir) / cmd)) {
			return true;
		}
	}
	return false;
}

extern "C" {
	syspower* syspower_create(const std::map<std::string, std::map<std::string, std::string>>& cfg) {
		return new syspower(cfg);
	}
}
