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

#ifndef __POPPLER_TEST_UTILS_H__
#define __POPPLER_TEST_UTILS_H__

#include <poppler.h>

G_BEGIN_DECLS

typedef void (*PopplerErrorFunc) (int position, char *message, va_list args);

const gchar *poppler_test_get_base_dir (void);
void         poppler_test_error_to_string_function (GString *string,
                                                    int      position,
                                                    char    *message,
                                                    va_list  args);
void         poppler_test_error_to_stderr_function (int      position,
                                                    char    *message,
                                                    va_list  args);

void         poppler_test_set_error_function       (PopplerErrorFunc func);

G_END_DECLS

#endif
