#include "config_parser.hpp"
#include <fstream>
#include <sstream>

config_parser::config_parser(const std::string &filename) {
	std::ifstream file(filename);
	std::string line;
	std::string current_section;
	int n_hotkeys;

	available = file.is_open();

	if (available) {
		while (std::getline(file, line)) {
			line = trim(line);

			if (line.empty() || line[0] == ';' || line[0] == '#') {
				continue;
			}
			else if (line[0] == '[' && line[line.size() - 1] == ']') {
				if (!current_section.empty()) {
					data[current_section]["n_hotkeys"] = std::to_string(n_hotkeys);
				}
				current_section = line.substr(1, line.size() - 2);
				n_hotkeys = 0;
			}
			else {
				size_t delimiter_pos = line.find('=');
				if (delimiter_pos != std::string::npos) {
					std::string key = trim(line.substr(0, delimiter_pos));
					std::string value = trim(line.substr(delimiter_pos + 1));
					if (key == "hotkey") {
						key += std::to_string(++n_hotkeys);
					}
					data[current_section][key] = value;
				}
			}
		}
		if (!current_section.empty()) {
			data[current_section]["n_hotkeys"] = std::to_string(n_hotkeys);
		}
		file.close();
	}
	else {
		std::fprintf(stderr, "Unable to open file: %s\n", filename.c_str());
	}
}

std::string config_parser::trim(const std::string &str) {
	size_t first = str.find_first_not_of(' ');
	if (std::string::npos == first) {
		return str;
	}
	size_t last = str.find_last_not_of(' ');
	return str.substr(first, (last - first + 1));
}
