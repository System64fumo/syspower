#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <gtkmm.h>
#include <string>
#include <thread>

class syspower : public Gtk::Window {
	public:
		Gtk::Label label_status;
		Gtk::ProgressBar progressbar_sync;
		Gtk::Box box_buttons;

		sigc::connection timer_connection;

		double max_slider_value;

		bool on_timer_tick();
		syspower();
	
	private:
		Gtk::Box box_layout;
		Gtk::Button button_shutdown;
		Gtk::Button button_reboot;
		Gtk::Button button_logout;
		Gtk::Button button_cancel;

		std::thread thread_action;

		void button_shutdown_clicked();
		void button_reboot_clicked();
		void button_logout_clicked();
		void button_cancel_clicked();
};

inline syspower* win;

#endif
