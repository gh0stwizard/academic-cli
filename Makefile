CURL_MODS = libcurl
CURL_CFLAGS = $(shell pkg-config --cflags $(CURL_MODS))
CURL_LIBS = $(shell pkg-config --libs $(CURL_MODS))

UV_MODS = libuv
UV_CFLAGS = $(shell pkg-config --cflags $(UV_MODS))
UV_LIBS = $(shell pkg-config --libs $(UV_MODS))

CFLAGS ?= -Wall $(CURL_CFLAGS) $(UV_CFLAGS)
CFLAGS += -D_POSIX_C_SOURCE=200809L
CFLAGS += -D_XOPEN_SOURCE=500
CFLAGS += -D_GNU_SOURCE
LDFLAGS ?= 
LIBS ?= $(CURL_LIBS) $(UV_LIBS)

TARGET = academic-cli
SOURCES = $(wildcard *.c)
OBJECTS = $(patsubst %.c, %.o, $(SOURCES))

all: $(TARGET)

devel: CFLAGS += -g -D_DEBUG -Wextra
devel: CFLAGS += -D_DEBUG_TERM
devel: all

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBS)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	$(RM) $(TARGET) $(OBJECTS)

.PHONY: all devel clean
