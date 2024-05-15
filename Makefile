all: syspower

clean:
	rm syspower ./*.o

sync.o: src/sync.cpp
	g++ -o sync.o -c src/sync.cpp \
	-Os -s -Wall

kill.o: src/kill.cpp
	g++ -o kill.o -c src/kill.cpp \
	-Os -s -Wall

window.o: src/window.cpp
	g++ -o window.o -c src/window.cpp \
	$$(pkg-config gtkmm-4.0 --cflags --libs) \
	$$(pkg-config gtk4-layer-shell-0 --cflags --libs) \
	-Os -s -Wall

main.o: src/main.cpp
	g++ -o main.o -c src/main.cpp \
	$$(pkg-config gtkmm-4.0 --cflags --libs) \
	$$(pkg-config gtk4-layer-shell-0 --cflags --libs) \
	-Os -s -Wall

syspower: main.o window.o sync.o kill.o
	g++ -o syspower main.o window.o sync.o kill.o \
	$$(pkg-config gtkmm-4.0 --cflags --libs) \
	$$(pkg-config gtk4-layer-shell-0 --cflags --libs) \
	-Os -s -Wall
