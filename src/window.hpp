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
		syspower(const config &cfg);
		void show_other_windows();

	private:
		config config_main;
		double max_slider_value;
		char command[30] = "";
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

		GdkDisplay *display;
		GListModel *monitors;
		std::vector<std::shared_ptr<Gtk::Window>> windows;

		void action_thread();
		void on_button_clicked(const char &button);
		bool on_timer_tick();
};

extern "C" {
	syspower *syspower_create(const config &cfg);
	void syspower_show_windows(syspower* window);
}

