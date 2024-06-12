#pragma once

#include <gtkmm/application.h>

inline Glib::RefPtr<Gtk::Application> app;
int get_dirty_pages();
void sync_filesystems();
void kill_child_processes();
