#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>

using namespace std;

string executeCommand(const string& command) {
	string result;
	char buffer[128];
	FILE* pipe = popen(command.c_str(), "r");
	if (!pipe) {
		cerr << "Error: Couldn't open pipe." << endl;
		return "";
	}
	while (!feof(pipe)) {
		if (fgets(buffer, 128, pipe) != nullptr)
			result += buffer;
	}
	pclose(pipe);
	return result;
}

// TODO: Rename this to kill_child_processes,
// Then start killing every child one by one (Morbid)
void print_child_processes() {
	// TODO: Consider adding support for other comps. (Probably won't happen)
	// Also consider checking if the program exists before running it
	// Also also maybe consider automatically checking what session the user is
	// running on?

	string command = "hyprctl clients -j";
	string output = executeCommand(command);


	string searchString = "pid";
	size_t pos = output.find(searchString);

	// Itterate over the output
	while (pos != string::npos) {
		size_t start = output.find(':', pos);
		if (start != string::npos) {
			size_t end = output.find_first_of(",}", start);
			if (end != string::npos) {
				cout << output.substr(start + 2, end - start - 2) << endl;
			} else {
				cerr << "Error: Couldn't find end of the value." << endl;
			}
		} else {
			cerr << "Error: Couldn't find start of the value." << endl;
		}
		pos = output.find(searchString, pos + 1);
	}
}
