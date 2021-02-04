CFLAGS ?= -O2
PKG_CONFIG ?= pkg-config
RM ?= rm -f
WITH_STATIC_MYHTML ?= YES
WITH_STATIC_SQLITE ?= NO
MYCFLAGS = -Wall -std=gnu99 -pedantic
MYCFLAGS += -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=500 -D_GNU_SOURCE

MAJOR_VERSION = 0
MINOR_VERSION = 2
PATCH_VERSION = 0
VERSION = $(MAJOR_VERSION).$(MINOR_VERSION).$(PATCH_VERSION)

DEFS_CURL = $(shell $(PKG_CONFIG) --cflags libcurl)
LIBS_CURL = $(shell $(PKG_CONFIG) --libs libcurl)

DEFS_UV = $(shell $(PKG_CONFIG) --cflags libuv)
LIBS_UV = $(shell $(PKG_CONFIG) --libs libuv)

DEFS_SQLITE = $(shell $(PKG_CONFIG) --cflags sqlite3)
LIBS_SQLITE = $(shell $(PKG_CONFIG) --libs sqlite3)
LIBS_SQLITE_STATIC = $(shell $(PKG_CONFIG) --libs --static sqlite3)
ifeq ($(WITH_STATIC_SQLITE),YES)
LIBS_SQLITE = -Wl,-Bstatic $(LIBS_SQLITE_STATIC) -Wl,-Bdynamic
endif

DEFS_MYHTML = -Imyhtml/include
LIBS_MYHTML = -Lmyhtml/lib -lmyhtml
ifeq ($(WITH_STATIC_MYHTML),YES)
LIBS_MYHTML = -Wl,-Bstatic -Lmyhtml/lib -lmyhtml_static -Wl,-Bdynamic -lpthread
endif
LIBS_STATIC_MYHTML = -Lmyhtml/lib -lmyhtml_static

TARGET = academic-cli
SOURCES = $(wildcard *.c)
OBJECTS = $(patsubst %.c, %.o, $(SOURCES))

#----------------------------------------------------------#

DEFS = $(DEFS_MYHTML) $(DEFS_CURL) $(DEFS_UV) $(DEFS_SQLITE)
LIBS += $(LIBS_MYHTML) $(LIBS_CURL) $(LIBS_UV) $(LIBS_SQLITE)
MYCFLAGS += $(DEFS) $(INCLUDES) -DAPP_VERSION=$(VERSION)

#----------------------------------------------------------#

all: $(TARGET)

devel: CFLAGS += -g -D_DEBUG
#devel: CFLAGS += -D_DEBUG_CLI
#devel: CFLAGS += -D_DEBUG_HTML
#devel: CFLAGS += -D_DEBUG_CURL
devel: all

$(TARGET): myhtml $(OBJECTS)
	# shared linkage
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBS)

%.o: %.c
	$(CC) $(MYCFLAGS) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<

clean:
	# cleanup
	$(RM) $(TARGET) $(OBJECTS)

distclean: clean myhtml-clean

strip: $(TARGET)
	# strip
	strip --strip-unneeded -R .comment -R .note -R .note.ABI-tag $(TARGET)

myhtml:
	$(MAKE) -C myhtml

myhtml-clean:
	# myhtml-clean
	$(MAKE) -C myhtml clean
	$(RM) myhtml/myhtml.pc

help:
	$(info WITH_STATIC_MYHTML = YES|NO - link with libmyhtml statically, default: YES)
	$(info WITH_STATIC_SQLITE = YES|NO - link with sqlite statically, default: NO)

.SILENT: help
.PHONY: all devel clean myhtml distclean strip myhtml-clean
