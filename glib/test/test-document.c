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
void test_property (void);

static PopplerDocument *document;

static gchar *tmp_base_dir;
static gchar *tmp_dir;

void
setup (void)
{
  document = NULL;

  cut_set_fixture_data_dir (poppler_test_get_base_dir (),
                            "fixtures",
                            "pdf",
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
  g_free (copy_path);

  poppler_document_save_a_copy (document, copy_uri, &error);
  gcut_assert_error (error);

  g_object_unref (document);
  document = poppler_document_new_from_file (copy_uri, NULL, &error);
  gcut_assert_error (error);
  cut_assert_equal_int (3, poppler_document_get_n_pages (document));
}

static gchar *
inspect_property (const gchar *title,
                  const gchar *format,
                  const gchar *author,
                  const gchar *subject,
                  const gchar *keywords,
                  GTime creation_date,
                  GTime mod_date,
                  const gchar *creator,
                  const gchar *producer,
                  const gchar *linearized,
                  PopplerPageMode mode,
                  PopplerPageLayout layout,
                  PopplerViewerPreferences viewer_preferences,
                  PopplerPermissions permissions)
{
  GString *inspected;
  gchar *inspected_mode, *inspected_layout;
  gchar *inspected_viewer_preferences, *inspected_permissions;

  inspected_mode = gcut_enum_inspect (POPPLER_TYPE_PAGE_MODE, mode);
  inspected_layout = gcut_enum_inspect (POPPLER_TYPE_PAGE_LAYOUT, layout);
  inspected_viewer_preferences =
    gcut_flags_inspect (POPPLER_TYPE_VIEWER_PREFERENCES, viewer_preferences);
  inspected_permissions =
    gcut_flags_inspect (POPPLER_TYPE_PERMISSIONS, permissions);

  inspected = g_string_new (NULL);
  g_string_append_printf (inspected,
                          "{"
                          "title=<%s>, "
                          "format=<%s>, "
                          "author=<%s>, "
                          "subject=<%s>, "
                          "keywords=<%s>, "
                          "creation-date=<%u>, "
                          "mod-date=<%u>, "
                          "creator=<%s>, "
                          "producer=<%s>, "
                          "linearized=<%s>, "
                          "mode=<%s>, "
                          "layout=<%s>, "
                          "viewer-preferences=<%s>, "
                          "permissions=<%s>"
                          "}",
                          title,
                          format,
                          author,
                          subject,
                          keywords,
                          creation_date,
                          mod_date,
                          creator,
                          producer,
                          linearized,
                          inspected_mode,
                          inspected_layout,
                          inspected_viewer_preferences,
                          inspected_permissions);
  g_free(inspected_mode);
  g_free(inspected_layout);
  g_free(inspected_viewer_preferences);
  g_free(inspected_permissions);

  return g_string_free (inspected, FALSE);
}

#define cut_poppler_assert_equal_property(...)                          \
  cut_trace(cut_poppler_assert_equal_property_helper(__VA_ARGS__))

static void
cut_poppler_assert_equal_property_helper (const gchar *title,
                                          const gchar *format,
                                          const gchar *author,
                                          const gchar *subject,
                                          const gchar *keywords,
                                          GTime creation_date,
                                          GTime mod_date,
                                          const gchar *creator,
                                          const gchar *producer,
                                          const gchar *linearized,
                                          PopplerPageMode mode,
                                          PopplerPageLayout layout,
                                          PopplerViewerPreferences viewer_preferences,
                                          PopplerPermissions permissions,
                                          PopplerDocument *document)
{
  gchar *actual_title;
  gchar *actual_format;
  gchar *actual_author;
  gchar *actual_subject;
  gchar *actual_keywords;
  GTime actual_creation_date;
  GTime actual_mod_date;
  gchar *actual_creator;
  gchar *actual_producer;
  gchar *actual_linearized;
  PopplerPageMode actual_mode;
  PopplerPageLayout actual_layout;
  PopplerViewerPreferences actual_viewer_preferences;
  PopplerPermissions actual_permissions;

  g_object_get (document,
		"title", &actual_title,
		"format", &actual_format,
		"author", &actual_author,
		"subject", &actual_subject,
		"keywords", &actual_keywords,
		"creation-date", &actual_creation_date,
		"mod-date", &actual_mod_date,
		"creator", &actual_creator,
		"producer", &actual_producer,
		"linearized", &actual_linearized,
		"page-mode", &actual_mode,
		"page-layout", &actual_layout,
		"viewer-preferences", &actual_viewer_preferences,
		"permissions", &actual_permissions,
		NULL);

  if (cut_equal_string (title, actual_title) &&
      cut_equal_string (format, actual_format) &&
      cut_equal_string (author, actual_author) &&
      cut_equal_string (subject, actual_subject) &&
      cut_equal_string (keywords, actual_keywords) &&
      creation_date == actual_creation_date &&
      mod_date == actual_mod_date &&
      cut_equal_string (creator, actual_creator) &&
      cut_equal_string (producer, actual_producer) &&
      cut_equal_string (linearized, actual_linearized) &&
      mode == actual_mode &&
      layout == actual_layout &&
      viewer_preferences == actual_viewer_preferences &&
      permissions == actual_permissions)
    {
      cut_test_pass ();
    }
  else
    {
      gchar *inspected_expected;
      gchar *inspected_actual;
      const gchar *message;

      inspected_expected = inspect_property (title,
                                             format,
                                             author,
                                             subject,
                                             keywords,
                                             creation_date,
                                             mod_date,
                                             creator,
                                             producer,
                                             linearized,
                                             mode,
                                             layout,
                                             viewer_preferences,
                                             permissions);
      inspected_actual = inspect_property (actual_title,
                                           actual_format,
                                           actual_author,
                                           actual_subject,
                                           actual_keywords,
                                           actual_creation_date,
                                           actual_mod_date,
                                           actual_creator,
                                           actual_producer,
                                           actual_linearized,
                                           actual_mode,
                                           actual_layout,
                                           actual_viewer_preferences,
                                           actual_permissions);
      message = cut_take_printf( "expected: <%s>\n"
                                 "  actual: <%s>",
                                 inspected_expected, inspected_actual);
      message = cut_append_diff (message, inspected_expected, inspected_actual);
      cut_fail("%s", message);
    }
}

void
test_property (void)
{
  GError *error = NULL;
  const gchar *uri;
  gchar *copy_path;
  const gchar *copy_uri;

  uri = build_uri ("property.pdf");
  document = poppler_document_new_from_file (uri, NULL, &error);
  gcut_assert_error (error);

  cut_poppler_assert_equal_property ("Property Document", "PDF-1.4", NULL,
                                     "property", "property, test",
                                     1223783560, 0,
                                     "Writer", "OpenOffice.org 2.4",
                                     "No",
                                     POPPLER_PAGE_MODE_USE_THUMBS,
                                     POPPLER_PAGE_LAYOUT_UNSET,
                                     0,
                                     POPPLER_PERMISSIONS_FULL,
                                     document);
}
