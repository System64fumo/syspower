#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

int get_dirty_pages() {
	ifstream meminfo("/proc/meminfo");
	string line;
	int dirtyValue = 0;

	if (meminfo.is_open()) {
		while (getline(meminfo, line)) {
			if (line.find("Dirty:") != string::npos) {
				stringstream ss(line.substr(16));
				ss >> dirtyValue;
				break;
			}
		}
		meminfo.close();
		return dirtyValue;
	}
	else {
		cerr << "Unable to open /proc/meminfo" << endl;
		return -1;
	}
}

void sync_filesystems() {
	sync();
}
