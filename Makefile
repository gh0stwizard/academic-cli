CURL_MODS = libcurl
CURL_CFLAGS = $(shell pkg-config --cflags $(CURL_MODS))
CURL_LIBS = $(shell pkg-config --libs $(CURL_MODS))

UV_MODS = libuv
UV_CFLAGS = $(shell pkg-config --cflags $(UV_MODS))
UV_LIBS = $(shell pkg-config --libs $(UV_MODS))

MYHTML_CFLAGS = -Imyhtml/include
MYHTML_LIBS = -Lmyhtml/lib -lmyhtml
MYHTML_LIBS_STATIC = $(MYHTML_LIBS)_static

SQLITE_MODS = sqlite3
SQLITE_CFLAGS = $(shell pkg-config --cflags $(SQLITE_MODS))
SQLITE_LIBS = $(shell pkg-config --libs $(SQLITE_MODS))
SQLITE_LIBS_STATIC = -Wl,-Bstatic,$(SQLITE_LIBS) -Wl,-Bdynamic

CFLAGS ?= 
CFLAGS += -Wall -std=gnu99
CFLAGS += $(MYHTML_CFLAGS) $(CURL_CFLAGS) $(UV_CFLAGS)
CFLAGS += -D_POSIX_C_SOURCE=200809L
CFLAGS += -D_XOPEN_SOURCE=500
CFLAGS += -D_GNU_SOURCE
LDFLAGS ?= 
LIBS ?= 
LIBS += $(MYHTML_LIBS) $(CURL_LIBS) $(UV_LIBS) $(SQLITE_LIBS)

TARGET = academic-cli
SOURCES = $(wildcard *.c)
OBJECTS = $(patsubst %.c, %.o, $(SOURCES))

all: $(TARGET)

static: LDFLAGS = -static
static: CURL_CFLAGS = $(shell pkg-config --static --cflags $(CURL_MODS))
static: CURL_LIBS = $(shell pkg-config --static --libs $(CURL_MODS))
static: SQLITE_CFLAGS = $(shell pkg-config --static --cflags $(SQLITE_MODS))
static: SQLITE_LIBS = $(shell pkg-config --static --libs $(SQLITE_MODS))
static: LIBS = $(MYHTML_LIBS_STATIC) $(CURL_LIBS) $(UV_LIBS) $(SQLITE_LIBS)
static: $(OBJECTS)
	# static linkage
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

devel: CFLAGS += -g -D_DEBUG
#devel: CFLAGS += -D_DEBUG_HTML
#devel: CFLAGS += -D_DEBUG_CURL
devel: all

$(TARGET): $(OBJECTS)
	# shared linkage
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) $(MYHTML_LIBS) $(LIBS)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	# cleanup
	$(RM) $(TARGET) $(OBJECTS)

strip: $(TARGET)
	# strip
	strip --strip-unneeded -R .comment -R .note -R .note.ABI-tag $(TARGET)

myhtml: myhtml-library

myhtml-patch: myhtml-revert-patch
	# myhtml-patch
	cd myhtml; \
	patch -p1 -N -i ../patches/myhtml_01.diff

myhtml-revert-patch:
	# myhtml-revert-patch
	cd myhtml; git checkout Makefile.cfg

myhtml-static:
	# myhtml-static
	cd myhtml; $(MAKE) static

myhtml-shared:
	# myhtml-shared
	cd myhtml; $(MAKE) shared

myhtml-library:
	# myhtml-library
	cd myhtml; $(MAKE) library

myhtml-clean:
	# myhtml-clean
	cd myhtml; $(MAKE) clean; rm -f myhtml.pc

.PHONY: all devel clean myhtml static full-static
