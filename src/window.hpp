#pragma once
#include "config.hpp"

#include <gtkmm/window.h>
#include <gtkmm/label.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/revealer.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>

class syspower : public Gtk::Window {
	public:
		syspower(const std::map<std::string, std::map<std::string, std::string>>&);
		void show_other_windows();

	private:
		std::map<std::string, std::map<std::string, std::string>> config_main;
		double max_slider_value;
		std::string command;
		Glib::ustring button_text;
		sigc::connection timer_connection;

		Gtk::Label label_status;
		Gtk::ProgressBar progressbar_sync;
		Gtk::Box box_layout;
		Gtk::Revealer revealer_box;
		Gtk::Box box_buttons;

		Gtk::Button button_shutdown;
		Gtk::Button button_reboot;
		Gtk::Button button_logout;
		Gtk::Button button_cancel;

		GdkDisplay* display;
		GListModel* monitors;
		std::vector<std::shared_ptr<Gtk::Window>> windows;

		void action_thread();
		void add_button(const std::string&);
		void on_button_clicked(const std::string&);
		bool on_timer_tick();
		bool systemd(const std::string&);
};

extern "C" {
	syspower *syspower_create(const std::map<std::string, std::map<std::string, std::string>>&);
}

