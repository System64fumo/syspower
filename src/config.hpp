#pragma once

// Build time configuration		Description
#define CONFIG_FILE				// Allow the use of a config file
#define CONFIG_RUNTIME			// Allow the use of runtime arguments

// Default config
struct config_power {
	int position = 4;
	int main_monitor = 0;
	int transition_duration = 1000;
};

