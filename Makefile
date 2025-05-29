CC = gcc
CFLAGS = -fdiagnostics-color=always -g
PKG_CONFIG_LIBS = libnotify glib-2.0 sdl2 SDL2_image SDL2_gfx SDL2_ttf SDL2_mixer

INCLUDES = -I./include $(shell pkg-config --cflags $(PKG_CONFIG_LIBS))
LIBS = $(shell pkg-config --libs $(PKG_CONFIG_LIBS))

SRC = $(wildcard src/*.c)
OUT = game

all:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC) -o $(OUT) $(LIBS)

clean:
	rm -f $(OUT)
