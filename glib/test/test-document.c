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
void test_new_from_data (void);
void test_save (void);
void test_save_a_copy (void);

static PopplerDocument *document;

static gchar *tmp_base_dir;
static gchar *tmp_dir;

void
setup (void)
{
  document = NULL;

  cut_set_fixture_data_dir (poppler_test_get_base_dir (),
                            "fixtures",
                            "document",
                            NULL);

  tmp_base_dir = g_build_filename (poppler_test_get_base_dir (), "tmp", NULL);
  cut_remove_path (tmp_base_dir);

  tmp_dir = g_build_filename (tmp_base_dir, "document", NULL);
  g_mkdir_with_parents (tmp_dir, 0755);
}

void
teardown (void)
{
  if (document)
    g_object_unref (document);

  if (tmp_dir)
    g_free (tmp_dir);

  if (tmp_base_dir)
    {
      cut_remove_path (tmp_base_dir);
      g_free (tmp_base_dir);
    }
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

void
test_new_from_data (void)
{
  GError *error = NULL;
  gchar *data;
  gsize length;
  const gchar *path;

  path = cut_take_string (cut_build_fixture_data_path ("multi-pages.pdf", NULL));
  g_file_get_contents (path, &data, &length, &error);
  gcut_assert_error (error);

  document = poppler_document_new_from_data (data, length, NULL, &error);
  g_free (data);
  gcut_assert_error (error);

  cut_assert_equal_int (3, poppler_document_get_n_pages (document));
}

void
test_save (void)
{
  GError *error = NULL;
  const gchar *uri;
  gchar *output_path;
  const gchar *output_uri;

  uri = build_uri ("multi-pages.pdf");
  document = poppler_document_new_from_file (uri, NULL, &error);
  gcut_assert_error (error);
  cut_assert_equal_int (3, poppler_document_get_n_pages (document));

  output_path = g_build_filename (tmp_dir, "saved-multi-pages.pdf", NULL);
  output_uri = cut_take_printf ("file://%s", output_path);
  g_free (output_path);

  poppler_document_save (document, output_uri, &error);
  gcut_assert_error (error);

  g_object_unref (document);
  document = poppler_document_new_from_file (output_uri, NULL, &error);
  gcut_assert_error (error);
  cut_assert_equal_int (3, poppler_document_get_n_pages (document));
}

void
test_save_a_copy (void)
{
  GError *error = NULL;
  const gchar *uri;
  gchar *copy_path;
  const gchar *copy_uri;

  uri = build_uri ("multi-pages.pdf");
  document = poppler_document_new_from_file (uri, NULL, &error);
  gcut_assert_error (error);
  cut_assert_equal_int (3, poppler_document_get_n_pages (document));

  copy_path = g_build_filename (tmp_dir, "copied-multi-pages.pdf", NULL);
  copy_uri = cut_take_printf ("file://%s", copy_path);
  g_print("%s\n", copy_uri);
  g_free (copy_path);

  poppler_document_save_a_copy (document, copy_uri, &error);
  gcut_assert_error (error);

  g_object_unref (document);
  document = poppler_document_new_from_file (copy_uri, NULL, &error);
  gcut_assert_error (error);
  cut_assert_equal_int (3, poppler_document_get_n_pages (document));
}
