CC = gcc
TEST_FLAGS = -g -fsanitize=address,undefined -Wall
FLAGS = $(shell pkg-config --cflags gtk4 gtk4-layer-shell-0 fftw3f)
LIBS = $(shell pkg-config --libs gtk4 gtk4-layer-shell-0 libpulse-simple) -lfftw3f -lm -lpthread
TARGET = visualizer

SOURCES = $(wildcard ./src/*.c) 

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(SOURCES) -o $(TARGET) $(FLAGS) $(LIBS)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	mkdir -p /usr/local/bin
	cp $(TARGET) /usr/local/bin/

uninstall:
	rm -f /usr/local/bin/$(TARGET)

test:
	$(CC) $(TEST_FLAGS) $(FLAGS) $(SOURCES) -o $(TARGET) $(LIBS)
