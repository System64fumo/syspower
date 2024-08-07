#pragma once
#include "config.hpp"
#include "window.hpp"

config_power config_main;
typedef syspower* (*syspower_create_func)(const config_power &cfg);
typedef void (*syspower_show_windows_func)(syspower*);

syspower_create_func syspower_create_ptr;
syspower_show_windows_func syspower_show_windows_ptr;
