/* -*- c-file-style: "gnu" -*- */
/*
 * Copyright (C) 2009  Kouhei Sutou <kou@cozmixng.org>
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

void test_title (void);
void test_visible (void);

static PopplerDocument *document;

void
setup (void)
{
  document = NULL;
  cut_set_fixture_data_dir (poppler_test_get_base_dir (),
                            "fixtures",
                            "pdf",
                            NULL);
}

void
teardown (void)
{
  if (document)
    g_object_unref (document);
}

static const GList *
load_layers (const gchar *fixture_data_component)
{
  PopplerLayersIter *iter;
  GList *layers = NULL;
  PopplerLayer *layer;
  GError *error = NULL;
  gchar *uri, *path;

  path = cut_build_fixture_data_path (fixture_data_component, NULL);
  uri = g_strconcat ("file://", path, NULL);
  g_free (path);

  document = poppler_document_new_from_file (uri, NULL, &error);
  g_free (uri);

  gcut_assert_error (error);

  iter = poppler_layers_iter_new (document);
  do {
    layers = g_list_append (layers, poppler_layers_iter_get_layer (iter));
  } while (poppler_layers_iter_next (iter));
  poppler_layers_iter_free (iter);

  return gcut_take_list (layers, (CutDestroyFunction)g_object_unref);
}

void
test_title (void)
{
  const GList *layers, *node;
  GList *titles = NULL;
  const GList *expected_titles;

  layers = load_layers ("layers.pdf");

  for (node = layers; node; node = g_list_next (node))
    {
      PopplerLayer *layer = node->data;
      titles = g_list_append (titles,
                              g_strdup (poppler_layer_get_title (layer)));
    }

  expected_titles = gcut_take_new_list_string ("layer2", "layer1", "layer0",
                                               NULL);
  gcut_assert_equal_list_string (expected_titles,
                                 gcut_take_list (titles, g_free));
}

void
test_visible (void)
{
  const GList *layers, *node;
  GList *visibilities = NULL;
  const GList *expected_visibilities;

  layers = load_layers ("layers.pdf");

  for (node = layers; node; node = g_list_next (node))
    {
      PopplerLayer *layer = node->data;

      if (poppler_layer_is_visible (layer))
        visibilities = g_list_append (visibilities, g_strdup ("visible"));
      else
        visibilities = g_list_append (visibilities, g_strdup ("hidden"));
    }

  expected_visibilities = gcut_take_new_list_string ("visible",
                                                     "hidden",
                                                     "visible",
                                                     NULL);
  gcut_assert_equal_list_string (expected_visibilities,
                                 gcut_take_list (visibilities, g_free));
}
