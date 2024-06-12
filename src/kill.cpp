#include <iostream>
#include <unistd.h>

std::string executeCommand(const std::string& command) {
	std::string result;
	char buffer[128];
	FILE* pipe = popen(command.c_str(), "r");
	while (!feof(pipe)) {
		if (fgets(buffer, 128, pipe) != nullptr)
			result += buffer;
	}
	pclose(pipe);
	return result;
}

void kill_child_processes() {
	// TODO: Consider adding support for other comps. (Probably won't happen)

	std::string command = "hyprctl clients -j";
	std::string output = executeCommand(command);
	pid_t own_pid = getpid();

	std::string searchString = "pid";
	size_t pos = output.find(searchString);

	// Itterate over the output
	while (pos != std::string::npos) {
		size_t start = output.find(':', pos);
		if (start != std::string::npos) {
			size_t end = output.find_first_of(",}", start);
			if (end != std::string::npos) {
				std::string pid = output.substr(start + 2, end - start - 2);
				std::string kill_command = "hyprctl dispatch closewindow pid:";
				kill_command += pid;

				// This is not really needed,
				// Since the program itself is a layershell.
				// But it might be helpful for when and if i get to adding
				// support for other compositors

				// Don't kill own process
				if (std::stoi(pid) == own_pid)
					continue;

				// TODO: Wait for the program to close
				// Also change the system status to "Closing xyz..."
				// And add a button to minimize the shutdown screen to take action
				// And a button to forcefully kill the program (Skip others too)
				system(kill_command.c_str());
			}
		}
		pos = output.find(searchString, pos + 1);
	}
}
