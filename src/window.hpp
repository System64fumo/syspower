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

	private:
		Gtk::Button button_shutdown;
		Gtk::Button button_reboot;
		Gtk::Button button_logout;
		Gtk::Button button_cancel;

		GdkDisplay *display;
		GListModel *monitors;

		void thread();
		void show_other_windows();
		void on_button_clicked(int button);
		bool on_timer_tick();
};

inline syspower* win;
