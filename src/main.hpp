#pragma once

#include "window.hpp"

#include <gtkmm/application.h>

inline Glib::RefPtr<Gtk::Application> app;
int get_dirty_pages();
void sync_filesystems();
void kill_child_processes();

typedef syspower* (*syspower_create_func)();
typedef void (*syspower_show_windows_func)(syspower*);

syspower_create_func syspower_create_ptr;
syspower_show_windows_func syspower_show_windows_ptr;
