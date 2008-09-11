if WITH_CUTTER
noinst_LTLIBRARIES =			\
	libpoppler-test-utils.la
endif

INCLUDES = 			\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/glib	\
	$(cairo_includes)	\
	$(POPPLER_GLIB_CFLAGS)	\
	$(GDK_CFLAGS)		\
	$(GCUTTER_CFLAGS)

LIBS =						\
	$(top_builddir)/glib/libpoppler-glib.la	\
	$(GCUTTER_LIBS)

libpoppler_test_utils_la_SOURCES =	\
	poppler-test-utils.c		\
	poppler-test-utils.h
