#pragma once

// Build time configuration		Description
#define RUNTIME_CONFIG			// Allow the use of runtime arguments

// Default config
struct config_power {
	int position = 4;
	int main_monitor = 0;
	int transition_duration = 1000;
};

