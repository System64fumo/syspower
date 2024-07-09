#pragma once

#include <gtkmm/window.h>
#include <gtkmm/label.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/revealer.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>

class syspower : public Gtk::Window {
	public:
		syspower();

		Gtk::Label label_status;
		Gtk::ProgressBar progressbar_sync;
		Gtk::Box box_layout;
		Gtk::Revealer revealer_box;
		Gtk::Box box_buttons;

		sigc::connection timer_connection;

		double max_slider_value;
		char command[30] = "";
		Glib::ustring button_text;

		void show_other_windows();

	private:
		Gtk::Button button_shutdown;
		Gtk::Button button_reboot;
		Gtk::Button button_logout;
		Gtk::Button button_cancel;

		GdkDisplay *display;
		GListModel *monitors;
		std::vector<std::shared_ptr<Gtk::Window>> windows;

		void thread();
		void on_button_clicked(int button);
		bool on_timer_tick();
};

extern "C" {
	syspower *syspower_create();
	void syspower_show_windows(syspower* window);
}

