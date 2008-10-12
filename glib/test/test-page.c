/* -*- c-file-style: "gnu" -*- */
/*
 * Copyright (C) 2008  Kouhei Sutou <kou@cozmixng.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <poppler.h>
#include "lib/poppler-test-utils.h"

#include <gcutter.h>
#ifdef CUT_SUPPORT_GDK_PIXBUF
#  include <gdkcutter-pixbuf.h>
#endif

void test_get_size (void);
void test_index_single_page (void);
void test_index_multi_pages (void);
void test_get_text (void);
void test_get_form_field_mapping (void);
void test_transition (void);
void test_render_pixbuf (void);

static PopplerPage *page;
static PopplerPageTransition *transition;
#ifdef CUT_SUPPORT_GDK_PIXBUF
static GdkPixbuf *expected_pixbuf, *actual_pixbuf;
#endif
static GList *fields;

void
setup (void)
{
  page = NULL;
  transition = NULL;
#ifdef CUT_SUPPORT_GDK_PIXBUF
  expected_pixbuf = NULL;
  actual_pixbuf = NULL;
#endif
  fields = NULL;

  cut_set_fixture_data_dir (poppler_test_get_base_dir (),
                            "fixtures",
                            NULL);
}

void
teardown (void)
{
  if (page)
    g_object_unref (page);

  if (transition)
    poppler_page_transition_free (transition);

#ifdef CUT_SUPPORT_GDK_PIXBUF
  if (expected_pixbuf)
    g_object_unref (expected_pixbuf);
  if (actual_pixbuf)
    g_object_unref (actual_pixbuf);
#endif

  if (fields)
    poppler_page_free_form_field_mapping (fields);
}

static PopplerPage *
load_page (const gchar *fixture_data_component, gint n)
{
  PopplerDocument *document;
  PopplerPage *page;
  GError *error = NULL;
  gchar *uri, *path;

  path = cut_build_fixture_data_path ("pdf", fixture_data_component, NULL);
  uri = g_strconcat ("file://", path, NULL);
  g_free (path);

  document = poppler_document_new_from_file (uri, NULL, &error);
  g_free (uri);

  gcut_assert_error (error);

  page = poppler_document_get_page (document, n);
  g_object_unref (document);
  return page;
}



void
test_get_size (void)
{
  double width, height;

  page = load_page ("simple-page.pdf", 0);

  poppler_page_get_size (page, &width, &height);
  cut_assert_equal_double (595, 0.1, width);
  cut_assert_equal_double (842, 0.1, height);
}

void
test_index_single_page (void)
{
  page = load_page ("simple-page.pdf", 0);

  cut_assert_equal_int (0, poppler_page_get_index (page));
}

void
test_index_multi_pages (void)
{
  page = load_page ("multi-pages.pdf", 1);

  cut_assert_equal_int (1, poppler_page_get_index (page));
}

void
test_get_text (void)
{
  PopplerRectangle rectangle;
  gchar *text;

  page = load_page ("simple-page.pdf", 0);

  rectangle.x1 = 0;
  rectangle.y1 = 0;
  poppler_page_get_size (page, &(rectangle.x2), &(rectangle.y2));
  text = poppler_page_get_text (page,
                                POPPLER_SELECTION_GLYPH,
                                &rectangle);
  cut_assert_equal_string_with_free ("Hello, World!", text);
}

void
test_get_form_field_mapping (void)
{
  page = load_page ("form.pdf", 0);

  fields = poppler_page_get_form_field_mapping (page);
  cut_assert_not_null (fields);
}

static gchar *
inspect_transition (PopplerPageTransitionType type,
                    PopplerPageTransitionAlignment alignment,
                    PopplerPageTransitionDirection direction,
                    gint duration,
                    gint angle,
                    gdouble scale,
                    gdouble rectangular)
{
  gchar *inspected;
  gchar *inspected_type, *inspected_alignment, *inspected_direction;

  inspected_type = gcut_enum_inspect (POPPLER_TYPE_PAGE_TRANSITION_TYPE, type);
  inspected_alignment =
    gcut_enum_inspect (POPPLER_TYPE_PAGE_TRANSITION_ALIGNMENT, alignment);
  inspected_direction =
    gcut_enum_inspect (POPPLER_TYPE_PAGE_TRANSITION_DIRECTION, direction);

  inspected = g_strdup_printf ("{"
                               "type=<%s>, "
                               "alignment=<%s>, "
                               "direction=<%s>, "
                               "duration=<%d>, "
                               "angle=<%d>, "
                               "scale=<%g>, "
                               "rectangular=<%g>, "
                               "}",
                               inspected_type,
                               inspected_alignment,
                               inspected_direction,
                               duration,
                               angle,
                               scale,
                               rectangular);
  g_free (inspected_type);
  g_free (inspected_alignment);
  g_free (inspected_direction);

  return inspected;
}

#define cut_poppler_assert_equal_transition(...)                       \
  cut_trace (cut_poppler_assert_equal_transition_helper (__VA_ARGS__))

static void
cut_poppler_assert_equal_transition_helper (PopplerPageTransitionType type,
                                            PopplerPageTransitionAlignment alignment,
                                            PopplerPageTransitionDirection direction,
                                            gint duration,
                                            gint angle,
                                            gdouble scale,
                                            gdouble rectangular,
                                            PopplerPageTransition *transition)
{
  if (type == transition->type &&
      alignment == transition->alignment &&
      direction == transition->direction &&
      duration == transition->duration &&
      angle == transition->angle &&
      cut_equal_double(scale, transition->scale, 0.1) &&
      cut_equal_double(rectangular, transition->rectangular, 0.1))
    {
      cut_test_pass();
    }
  else
    {
      const gchar *message;
      gchar *inspected_expected, *inspected_actual;

      inspected_expected = inspect_transition (type,
                                               alignment,
                                               direction,
                                               duration,
                                               angle,
                                               scale,
                                               rectangular);
      inspected_actual = inspect_transition (transition->type,
                                             transition->alignment,
                                             transition->direction,
                                             transition->duration,
                                             transition->angle,
                                             transition->scale,
                                             transition->rectangular);
      message = cut_take_printf ("expected: <%s>\n"
                                 "  actual: <%s>",
                                 inspected_expected, inspected_actual);
      message = cut_append_diff (message, inspected_expected, inspected_actual);
      g_free (inspected_expected);
      g_free (inspected_actual);
      cut_fail ("%s", message);
    }
}

void
test_transition (void)
{
  page = load_page ("slide.pdf", 1);

  transition = poppler_page_get_transition (page);
  cut_assert_not_null (transition);

  cut_poppler_assert_equal_transition (POPPLER_PAGE_TRANSITION_WIPE,
                                       POPPLER_PAGE_TRANSITION_HORIZONTAL,
                                       POPPLER_PAGE_TRANSITION_INWARD,
                                       1,
                                       180,
                                       1.0,
                                       0.0,
                                       transition);
}

#if defined(POPPLER_WITH_GDK) && defined(CUT_SUPPORT_GDK_PIXBUF)
static GdkPixbuf *
load_pixbuf (const gchar *path)
{
  GdkPixbuf *pixbuf;
  GError *error = NULL;
  gchar *pixbuf_path;

  pixbuf_path = cut_build_fixture_data_path ("png", path, NULL);
  pixbuf = gdk_pixbuf_new_from_file (pixbuf_path, &error);
  g_free (pixbuf_path);

  gcut_assert_error (error);

  return pixbuf;
}
#endif

void
test_render_pixbuf (void)
{
#if defined(POPPLER_WITH_GDK) && defined(CUT_SUPPORT_GDK_PIXBUF)
  double width, height;

  page = load_page ("shape.pdf", 0);
  poppler_page_get_size (page, &width, &height);

  expected_pixbuf = load_pixbuf ("shape.png");
  actual_pixbuf = gdk_pixbuf_new (gdk_pixbuf_get_colorspace (expected_pixbuf),
                                  gdk_pixbuf_get_has_alpha (expected_pixbuf),
                                  gdk_pixbuf_get_bits_per_sample (expected_pixbuf),
                                  width,
                                  height);
  gdk_pixbuf_fill (actual_pixbuf, 0xffffff00);
  poppler_page_render_to_pixbuf (page,
                                 0, 0, width, height, 1, 0,
                                 actual_pixbuf);
  gdkcut_pixbuf_assert_equal (expected_pixbuf, actual_pixbuf, 0x04);
#else
  cut_omit("gdk-pixbuf support isn't available");
#endif
}
