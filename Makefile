CXXFLAGS=-march=native -mtune=native -Os -s -Wall
DESTDIR=$(HOME)/.local

all: syspower

clean:
	rm syspower ./*.o

sync.o: src/sync.cpp
	g++ -o sync.o -c src/sync.cpp \
	$(CXXFLAGS)

kill.o: src/kill.cpp
	g++ -o kill.o -c src/kill.cpp \
	$(CXXFLAGS)

window.o: src/window.cpp
	g++ -o window.o -c src/window.cpp \
	$$(pkg-config gtkmm-4.0 --cflags --libs) \
	$$(pkg-config gtk4-layer-shell-0 --cflags --libs) \
	$(CXXFLAGS)

main.o: src/main.cpp
	g++ -o main.o -c src/main.cpp \
	$$(pkg-config gtkmm-4.0 --cflags --libs) \
	$$(pkg-config gtk4-layer-shell-0 --cflags --libs) \
	$(CXXFLAGS)

syspower: main.o window.o sync.o kill.o
	g++ -o syspower main.o window.o sync.o kill.o \
	$$(pkg-config gtkmm-4.0 --cflags --libs) \
	$$(pkg-config gtk4-layer-shell-0 --cflags --libs) \
	$(CXXFLAGS)

install: syspower
	mkdir -p $(DESTDIR)/bin
	install ./syspower $(DESTDIR)/bin/syspower
