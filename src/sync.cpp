#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>

int get_dirty_pages() {
	std::ifstream meminfo("/proc/meminfo");
	std::string line;
	int dirtyValue = 0;

	if (meminfo.is_open()) {
		while (getline(meminfo, line)) {
			if (line.find("Dirty:") != std::string::npos) {
				std::stringstream ss(line.substr(16));
				ss >> dirtyValue;
				break;
			}
		}
		meminfo.close();
		return dirtyValue;
	}
	else {
		std::cerr << "Unable to open /proc/meminfo" << std::endl;
		return -1;
	}
}

void sync_filesystems() {
	sync();
}
