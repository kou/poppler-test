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
void test_nested (void);

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

static void
load_document (const gchar *fixture_data_component)
{
  gchar *uri, *path;
  GError *error = NULL;

  path = cut_build_fixture_data_path (fixture_data_component, NULL);
  uri = g_strconcat ("file://", path, NULL);
  g_free (path);

  document = poppler_document_new_from_file (uri, NULL, &error);
  g_free (uri);

  gcut_assert_error (error);
}

static const GList *
load_layers (const gchar *fixture_data_component)
{
  PopplerLayersIter *iter;
  GList *layers = NULL;
  PopplerLayer *layer;

  load_document (fixture_data_component);

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

static GList *
collect_layer_titles (PopplerLayersIter *iter, const gchar *prefix)
{
  GList *titles = NULL;
  gint i = 0;

  if (!iter)
    return NULL;

  do {
    PopplerLayersIter *child_iter;
    gchar *title, *full_title;

    title = poppler_layers_iter_get_title (iter);
    if (!title)
      title = g_strdup_printf ("(non title: %d)", i);
    full_title = g_strdup_printf ("%s: %s", prefix, title);
    titles = g_list_append (titles, full_title);
    g_free (title);

    child_iter = poppler_layers_iter_get_child (iter);
    if (child_iter)
      {
        titles = g_list_concat (titles,
                                collect_layer_titles (child_iter, full_title));
        poppler_layers_iter_free (child_iter);
      }
    i++;
  } while (poppler_layers_iter_next (iter));

  return titles;
}

void
test_nested (void)
{
  PopplerLayersIter *iter;
  const GList *expected_titles;
  GList *actual_titles;

  load_document ("nested-layers.pdf");

  iter = poppler_layers_iter_new (document);
  actual_titles = collect_layer_titles (iter, "(top)");
  poppler_layers_iter_free (iter);
  gcut_take_list (actual_titles, g_free);

  expected_titles =
    gcut_take_new_list_string ("(top): (non title: 0)",
                               "(top): (non title: 1)",
                               "(top): (non title: 1): (non title: 0)",
                               "(top): (non title: 2)",
                               "(top): (non title: 3)",
                               NULL);
  gcut_assert_equal_list_string (expected_titles, actual_titles);
}
