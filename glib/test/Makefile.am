SUBDIRS =		\
	lib		\
	fixtures

if WITH_CUTTER
TESTS = run-test.sh
TESTS_ENVIRONMENT = NO_MAKE=yes CUTTER="$(CUTTER)"

noinst_LTLIBRARIES =		\
	test-document.la	\
	test-page.la		\
	test-form-field.la	\
	test-layer.la
endif

INCLUDES =				\
	-I$(top_srcdir)			\
	-I$(top_srcdir)/glib		\
	$(cairo_includes)		\
	$(POPPLER_GLIB_CFLAGS)		\
	$(GDK_CFLAGS)			\
	$(GCUTTER_CFLAGS)		\
	$(GDKCUTTER_PIXBUF_CFLAGS)

LIBS =							\
	$(top_builddir)/glib/libpoppler-glib.la		\
	$(GCUTTER_LIBS)					\
	$(GDKCUTTER_PIXBUF_LIBS)			\
	$(builddir)/lib/libpoppler-test-utils.la

AM_LDFLAGS =			\
	-module			\
	-rpath `pwd`/.libs	\
	-avoid-version

test_document_la_SOURCES = test-document.c
test_page_la_SOURCES = test-page.c
test_form_field_la_SOURCES = test-form-field.c
test_layer_la_SOURCES		= test-layer.c

echo-cutter:
	@echo $(CUTTER)
