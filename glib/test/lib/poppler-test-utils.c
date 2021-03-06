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

#include "poppler-test-utils.h"

static gchar *base_dir = NULL;
const gchar *
poppler_test_get_base_dir (void)
{
  const gchar *dir;

  if (base_dir)
    return base_dir;

  dir = g_getenv ("BASE_DIR");
  if (!dir)
    dir = ".";

  if (g_path_is_absolute (dir)) {
    base_dir = g_strdup (dir);
  } else {
    gchar *current_dir;

    current_dir = g_get_current_dir ();
    base_dir = g_build_filename (current_dir, dir, NULL);
    g_free (current_dir);
  }

  return base_dir;
}

void
poppler_test_error_to_string_function (GString *string,
                                       int      position,
                                       char    *message,
                                       va_list  args)
{
  if (position >= 0)
    {
      g_string_append_printf (string, "Error (%d): ", position);
    }
  else
    {
      g_string_append (string, "Error: ");
    }

  g_string_append_vprintf (string, message, args);
  g_string_append (string, "\n");
}

void
poppler_test_error_to_stderr_function (int      position,
                                       char    *message,
                                       va_list  args)
{
  GString *string;

  string = g_string_new (NULL);
  poppler_test_error_to_string_function (string, position, message, args);
  g_printerr ("%s", string->str);
  g_string_free (string, TRUE);
}
