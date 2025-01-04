#pragma once
#include "config.hpp"
#include "window.hpp"

typedef syspower* (*syspower_create_func)(const std::map<std::string, std::map<std::string, std::string>>&);
typedef void (*syspower_show_windows_func)(syspower*);

syspower_create_func syspower_create_ptr;
syspower_show_windows_func syspower_show_windows_ptr;
