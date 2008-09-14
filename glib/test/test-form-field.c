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

void test_general (void);
void test_push_button (void);
void test_check_button (void);
void test_radio_button (void);
void test_normal_text (void);
void test_normal_password_text (void);

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

static void
load_fields (void)
{
  page = load_page ("form.pdf", 0);

  fields = poppler_page_get_form_field_mapping (page);
  cut_assert_equal_int (10, g_list_length (fields));
}

void
test_general (void)
{
  PopplerFormFieldMapping *mapping;
  PopplerFormField *field;

  load_fields ();

  mapping = g_list_nth_data (fields, 4);
  field = mapping->field;
  cut_assert_equal_int (POPPLER_FORM_FIELD_CHOICE,
                        poppler_form_field_get_field_type (field));
  cut_assert_equal_int (65541, poppler_form_field_get_id (field));
  cut_assert_equal_double (0, 0.1, poppler_form_field_get_font_size (field));
  cut_assert_false (poppler_form_field_is_read_only (field));
}

void
test_push_button (void)
{
  PopplerFormFieldMapping *mapping;
  PopplerFormField *field;

  load_fields ();

  mapping = g_list_nth_data (fields, 7);
  field = mapping->field;
  cut_assert_equal_int (POPPLER_FORM_FIELD_BUTTON,
                        poppler_form_field_get_field_type (field));
  cut_assert_false (poppler_form_field_is_read_only (field));

  cut_assert_equal_int (POPPLER_FORM_BUTTON_PUSH,
                        poppler_form_field_button_get_button_type (field));
  cut_assert_false (poppler_form_field_button_get_state (field));
  poppler_form_field_button_set_state (field, TRUE);
  cut_assert_false (poppler_form_field_button_get_state (field));
}

void
test_check_button (void)
{
  PopplerFormFieldMapping *mapping;
  PopplerFormField *field;

  load_fields ();

  mapping = g_list_nth_data (fields, 9);
  field = mapping->field;
  cut_assert_equal_int (POPPLER_FORM_FIELD_BUTTON,
                        poppler_form_field_get_field_type (field));
  cut_assert_false (poppler_form_field_is_read_only (field));

  cut_assert_equal_int (POPPLER_FORM_BUTTON_CHECK,
                        poppler_form_field_button_get_button_type (field));
  cut_assert_true (poppler_form_field_button_get_state (field));
  poppler_form_field_button_set_state (field, FALSE);
  cut_assert_false (poppler_form_field_button_get_state (field));
}

void
test_radio_button (void)
{
  PopplerFormFieldMapping *mapping;
  PopplerFormField *field;

  load_fields ();

  mapping = g_list_nth_data (fields, 5);
  field = mapping->field;
  cut_assert_equal_int (POPPLER_FORM_FIELD_BUTTON,
                        poppler_form_field_get_field_type (field));
  cut_assert_false (poppler_form_field_is_read_only (field));

  cut_assert_equal_int (POPPLER_FORM_BUTTON_RADIO,
                        poppler_form_field_button_get_button_type (field));
  cut_assert_false (poppler_form_field_button_get_state (field));
  poppler_form_field_button_set_state (field, TRUE);
  cut_assert_true (poppler_form_field_button_get_state (field));
}

void
test_normal_text (void)
{
  PopplerFormFieldMapping *mapping;
  PopplerFormField *field;

  load_fields ();

  mapping = g_list_nth_data (fields, 3);
  field = mapping->field;
  cut_assert_equal_int (POPPLER_FORM_FIELD_TEXT,
                        poppler_form_field_get_field_type (field));
  cut_assert_false (poppler_form_field_is_read_only (field));

  cut_assert_equal_int (POPPLER_FORM_TEXT_NORMAL,
                        poppler_form_field_text_get_text_type (field));

  cut_assert_equal_string (NULL, poppler_form_field_text_get_text (field));
  poppler_form_field_text_set_text (field, "normal text");
  cut_assert_equal_string ("normal text",
                           poppler_form_field_text_get_text (field));

  cut_assert_equal_int (5, poppler_form_field_text_get_max_len (field));

  cut_assert_true (poppler_form_field_text_do_spell_check (field));
  cut_assert_true (poppler_form_field_text_do_scroll (field));
  cut_assert_false (poppler_form_field_text_is_rich_text (field));
  cut_assert_false (poppler_form_field_text_is_password (field));
}

void
test_normal_password_text (void)
{
  PopplerFormFieldMapping *mapping;
  PopplerFormField *field;

  load_fields ();

  mapping = g_list_nth_data (fields, 1);
  field = mapping->field;
  cut_assert_equal_int (POPPLER_FORM_FIELD_TEXT,
                        poppler_form_field_get_field_type (field));
  cut_assert_false (poppler_form_field_is_read_only (field));

  cut_assert_equal_int (POPPLER_FORM_TEXT_NORMAL,
                        poppler_form_field_text_get_text_type (field));

  cut_assert_equal_string ("password", poppler_form_field_text_get_text (field));
  poppler_form_field_text_set_text (field, "new password");
  cut_assert_equal_string ("new password",
                           poppler_form_field_text_get_text (field));

  cut_assert_equal_int (0, poppler_form_field_text_get_max_len (field));

  cut_assert_true (poppler_form_field_text_do_spell_check (field));
  cut_assert_true (poppler_form_field_text_do_scroll (field));
  cut_assert_false (poppler_form_field_text_is_rich_text (field));
  cut_assert_true (poppler_form_field_text_is_password (field));
}
