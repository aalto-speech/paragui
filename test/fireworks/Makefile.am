noinst_PROGRAMS = fireworks

fireworks_SOURCES = \
	blob.cpp \
	main.cpp

fireworks_LDADD = \
	$(SDL_LIBS) \
	-L../../src ../../src/libparagui.la \
	$(FREETYPE_LIBS) \
	$(LIBSTDCPP) \
	$(SIGC_LIBS)

EXTRA_DIST = \
	blob.h

INCLUDES = \
	-I$(top_srcdir)/include \
	$(SDL_CFLAGS) \
	$(SIGC_CFLAGS)
