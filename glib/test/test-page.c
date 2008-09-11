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

void test_size (void);

static PopplerPage *page;

void
setup (void)
{
  page = NULL;

  cut_set_fixture_data_dir (poppler_test_get_base_dir (),
                            "fixtures",
                            "page",
                            NULL);
}

void
teardown (void)
{
  if (page)
    g_object_unref (page);
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
test_size (void)
{
  PopplerRectangle rectangle;

  page = load_page ("simple-page.pdf", 0);

  rectangle.x1 = 0;
  rectangle.y1 = 0;
  poppler_page_get_size (page, &(rectangle.x2), &(rectangle.y2));
  cut_assert_equal_string ("Hello, World!",
                           poppler_page_get_text (page,
                                                  POPPLER_SELECTION_GLYPH,
                                                  &rectangle));
}
