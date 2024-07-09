#pragma once

// Build time configuration		Description
#define RUNTIME_CONFIG

/*
	Default config.
	Can be configured instead of using launch arguments.
	Runtime configuration can be disabled by deleting #define RUNTIME_CONFIG
*/

struct config {
	int position = 4;
	int main_monitor = 0;
	int transition_duration = 1000;
};

