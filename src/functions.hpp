#pragma once
#include <string>

class syspower_functions {
	public:
		static int get_dirty_pages();
		static void sync_filesystems();
		static void kill_child_processes();

	private:
		static std::string executeCommand(const std::string& command);
};
