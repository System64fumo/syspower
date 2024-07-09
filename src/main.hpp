#pragma once

#include "window.hpp"

typedef syspower* (*syspower_create_func)();
typedef void (*syspower_show_windows_func)(syspower*);

syspower_create_func syspower_create_ptr;
syspower_show_windows_func syspower_show_windows_ptr;
