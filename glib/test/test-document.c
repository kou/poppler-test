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

void test_new_from_file (void);

static PopplerDocument *document;

void
setup (void)
{
  document = NULL;

  cut_set_fixture_data_dir (poppler_test_get_base_dir (),
                            "fixtures",
                            "document",
                            NULL);
}

void
teardown (void)
{
  if (document)
    g_object_unref (document);
}

static const gchar *
build_uri (const gchar *fixture_data_component)
{
  gchar *uri, *path;

  path = cut_build_fixture_data_path (fixture_data_component, NULL);
  uri = g_strconcat ("file://", path, NULL);
  g_free (path);

  return cut_take_string (uri);
}

void
test_new_from_file (void)
{
  GError *error = NULL;
  const gchar *uri;

  uri = build_uri ("multi-pages.pdf");
  document = poppler_document_new_from_file (uri, NULL, &error);
  gcut_assert_error (error);

  cut_assert_equal_int (3, poppler_document_get_n_pages (document));
}
