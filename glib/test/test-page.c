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

void test_get_size (void);
void test_index_single_page (void);
void test_index_multi_pages (void);
void test_get_text (void);
void test_get_form_field_mapping (void);

static PopplerPage *page;
static GList *fields;

void
setup (void)
{
  page = NULL;
  fields = NULL;

  cut_set_fixture_data_dir (poppler_test_get_base_dir (),
                            "fixtures",
                            "pdf",
                            NULL);
}

void
teardown (void)
{
  if (page)
    g_object_unref (page);

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

  path = cut_build_fixture_data_path (fixture_data_component, NULL);
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
  cut_assert_equal_int (8, g_list_length (fields));
}
