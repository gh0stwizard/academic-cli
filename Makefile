CURL_MODS = libcurl
CURL_CFLAGS = $(shell pkg-config --cflags $(CURL_MODS))
CURL_LIBS = $(shell pkg-config --libs $(CURL_MODS))

UV_MODS = libuv
UV_CFLAGS = $(shell pkg-config --cflags $(UV_MODS))
UV_LIBS = $(shell pkg-config --libs $(UV_MODS))

MYHTML_LIBS = -Lmyhtml/lib -lmyhtml
MYHTML_LIBS_STATIC = -Wl,-Bstatic,$(MYHTML_LIBS) -Wl,-Bdynamic

CFLAGS ?= -Wall $(CURL_CFLAGS) $(UV_CFLAGS)
CFLAGS += -D_POSIX_C_SOURCE=200809L
CFLAGS += -D_XOPEN_SOURCE=500
CFLAGS += -D_GNU_SOURCE
LDFLAGS ?= 
LIBS ?= $(CURL_LIBS) $(UV_LIBS)

TARGET = academic-cli
SOURCES = $(wildcard *.c)
OBJECTS = $(patsubst %.c, %.o, $(SOURCES))

all: myhtml $(TARGET)

devel: CFLAGS += -g -D_DEBUG -Wextra
devel: all

$(TARGET): $(OBJECTS)
	# academic-cli
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) $(MYHTML_LIBS) $(LIBS)
#	# FIXME: libmyhtml.a throws error about undefined reference
#	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) $(MYHTML_LIBS_STATIC) $(LIBS)
#	cd myhtml; git checkout Makefile.cfg
#	rm myhtml/myhtml.pc

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

html.o: CFLAGS += -Imyhtml/include

clean: myhtml-clean
	$(RM) $(TARGET) $(OBJECTS)

myhtml: myhtml-patch myhtml-library

myhtml-patch: myhtml-revert-patch
	# myhtml-patch
	cd myhtml; \
	patch -p1 -N -i ../patch_myhtml_static.diff

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

myhtml-clean: myhtml-patch
	# myhtml-clean
	cd myhtml; $(MAKE) clean; rm myhtml.pc

.PHONY: all devel clean
