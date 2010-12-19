/*
 * This file is part of YAD.
 *
 * YAD is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * YAD is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with YAD; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Copyright (C) 2008-2010, Victor Ananjevsky <ananasik@gmail.com>
 *
 */

#include <config.h>

#include <gtk/gtk.h>
#include <glib/gi18n.h>

typedef struct {
  GtkWidget *win;
  GtkWidget *image;
  GtkWidget *label;
  GtkWidget *cat_list;
  GtkWidget *icon_list;

  GtkIconTheme *theme;

  GHashTable *icons;
} IconBrowserData;

static GtkListStore *
load_icon_cat (IconBrowserData *data, gchar *cat)
{
  GtkListStore *store;
  GList *i, *icons;
  gint size, w, h;

  gtk_icon_size_lookup (GTK_ICON_SIZE_MENU, &w, &h);
  size = MIN (w, h);

  store = gtk_list_store_new (2, GDK_TYPE_PIXBUF, G_TYPE_STRING);

  icons = gtk_icon_theme_list_icons (data->theme, cat);
  for (i = icons; i; i = i->next)
    {
      GtkTreeIter iter;
      GdkPixbuf *pb, *spb;

      spb = pb = gtk_icon_theme_load_icon (data->theme, i->data, size, 
					   GTK_ICON_LOOKUP_GENERIC_FALLBACK, NULL);
      
      if (pb)
	{
	  /* scale pixbuf if needed */
	  w = gdk_pixbuf_get_width (pb);
	  h = gdk_pixbuf_get_height (pb);
	  if (w > size || h > size)
	    {
	      pb = gdk_pixbuf_scale_simple (spb, size, size, GDK_INTERP_BILINEAR);
	      g_object_unref (spb);
	    }
	}

      gtk_list_store_append (store, &iter);
      gtk_list_store_set (store, &iter, 0, pb, 1, i->data, -1);

      if (pb)
	g_object_unref (pb);
      g_free (i->data);
    }
  g_list_free (icons);
  
  return store;
}

static void
select_icon (GtkTreeSelection *sel, IconBrowserData *data)
{
  GtkTreeModel *model;
  GtkTreeIter iter;
  GtkIconInfo *info;
  gint *sz, i = 0;
  gchar *icon, *lbl, *sizes = NULL, **s = NULL;

  if (!gtk_tree_selection_get_selected (sel, &model, &iter))
    return;

  gtk_tree_model_get (model, &iter, 1, &icon, -1);

  gtk_image_set_from_icon_name (GTK_IMAGE (data->image), icon, GTK_ICON_SIZE_DIALOG);

  sz = gtk_icon_theme_get_icon_sizes (data->theme, icon);
  info = gtk_icon_theme_lookup_icon (data->theme, icon, sz[0], 0);
  
  /* create sizes string */
  while (sz[i])
    {
      if (sz[i] == -1)
	{
	  sizes = g_strdup (_("scalable"));
	  break;
	}
      s = g_realloc (s, (i + 1) * sizeof (gchar *));
      s[i] = g_strdup_printf ("%dx%d", sz[i], sz[i]);
      i++;
      s[i] = NULL;
    }
  if (!sizes)
    sizes = g_strjoinv (" ", s);
  /* free memory */
  g_free (sz);
  i = 0;
  while (s && s[i])
    g_free (s[i++]);
  g_free (s);

  lbl = g_strdup_printf (_("<b>Name:</b> %s\n<b>Sizes:</b> %s\n<b>Filename:</b> %s"),
			 icon, sizes, gtk_icon_info_get_filename (info));
  gtk_label_set_markup (GTK_LABEL (data->label), lbl);
  g_free (sizes);
  g_free (lbl);
}

static void
select_cat (GtkTreeSelection *sel, IconBrowserData *data)
{
  GtkTreeModel *model;
  GtkListStore *store;
  GtkTreeIter iter;
  gchar *cat;

  if (!gtk_tree_selection_get_selected (sel, &model, &iter))
    return;

  gtk_tree_model_get (model, &iter, 0, &cat, -1);

  store = g_hash_table_lookup (data->icons, cat);
  if (!store)
    {
      store = load_icon_cat (data, cat);
      g_hash_table_insert (data->icons, cat, store);
    }
  gtk_tree_view_set_model (GTK_TREE_VIEW (data->icon_list), GTK_TREE_MODEL (store));
}

gint 
main (gint argc, gchar *argv[])
{
  IconBrowserData *data;
  gchar **themes = NULL;
  GList *ic, *icat;
  GtkListStore *store;
  GtkTreeSelection *sel;
  GtkTreeViewColumn *col;
  GtkCellRenderer *r;
  GtkWidget *w, *p, *box;

  GOptionEntry entrs[] = {
    { G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_STRING_ARRAY, &themes, NULL, NULL },
    { NULL }
  };

  data = g_new0 (IconBrowserData, 1);

#ifdef ENABLE_NLS
  bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);
#endif

  data = g_new0 (IconBrowserData, 1);

  /* initialize GTK+ and parse the command line arguments */
  gtk_init_with_args (&argc, &argv, _("- Icon browser"), entrs, GETTEXT_PACKAGE, NULL);

  /* load icon theme */
  if (themes && themes[0])
    {
      data->theme = gtk_icon_theme_new ();
      gtk_icon_theme_set_custom_theme (data->theme, themes[0]);
    }
  else
    data->theme = gtk_icon_theme_get_default ();

  /* create interface */
  data->win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (data->win), _("Icon browser"));
  gtk_window_set_icon_name (GTK_WINDOW (data->win), "gtk-info");
  gtk_window_set_default_size (GTK_WINDOW (data->win), 400, 300);
  g_signal_connect (G_OBJECT (data->win), "delete-event", gtk_main_quit, NULL);

  box = gtk_vbox_new (FALSE, 5);
  gtk_container_add (GTK_CONTAINER (data->win), box);
  gtk_container_set_border_width (GTK_CONTAINER (data->win), 5);

  /* create icon info box */
  w = gtk_hbox_new (FALSE, 5);
  gtk_box_pack_start (GTK_BOX (box), w, FALSE, TRUE, 2);

  data->image = gtk_image_new ();
  gtk_box_pack_start (GTK_BOX (w), data->image, FALSE, TRUE, 2);
  
  data->label = gtk_label_new ("");
  gtk_box_pack_start (GTK_BOX (w), data->label, TRUE, TRUE, 2);
  
  /* create icon browser */
  p = gtk_hpaned_new ();
  gtk_paned_set_position (GTK_PANED (p), 100);
  gtk_box_pack_start (GTK_BOX (box), p, TRUE, TRUE, 2);

  /* create category list */
  w = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (w),
				  GTK_POLICY_AUTOMATIC,
				  GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (w), GTK_SHADOW_ETCHED_IN);
  gtk_paned_add1 (GTK_PANED (p), w);

  store = gtk_list_store_new (1, G_TYPE_STRING);

  data->cat_list = gtk_tree_view_new_with_model (GTK_TREE_MODEL (store));
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (data->cat_list), TRUE);
  gtk_container_add (GTK_CONTAINER (w), data->cat_list);
  
  r = gtk_cell_renderer_text_new ();
  col = gtk_tree_view_column_new_with_attributes (_("Category"), r, "text", 0, NULL);
  gtk_tree_view_column_set_resizable (col, TRUE);
  gtk_tree_view_column_set_expand (col, TRUE);
  gtk_tree_view_append_column (GTK_TREE_VIEW (data->cat_list), col);

  /* load icons category */
  data->icons = g_hash_table_new (g_direct_hash, g_direct_equal);
  icat = gtk_icon_theme_list_contexts (data->theme);
  for (ic = icat; ic; ic = ic->next)
    {
      GtkTreeIter iter;

      gtk_list_store_append (store, &iter);
      gtk_list_store_set (store, &iter, 0, ic->data, -1);
      g_free (ic->data);
    }
  g_list_free (icat);

  /* create icons list */
  w = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (w),
				  GTK_POLICY_AUTOMATIC,
				  GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (w), GTK_SHADOW_ETCHED_IN);
  gtk_paned_add2 (GTK_PANED (p), w);

  data->icon_list = gtk_tree_view_new ();
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (data->icon_list), TRUE);
  gtk_container_add (GTK_CONTAINER (w), data->icon_list);

  col = gtk_tree_view_column_new ();
  r = gtk_cell_renderer_pixbuf_new ();
  gtk_tree_view_column_pack_start (col, r, FALSE);
  gtk_tree_view_column_set_attributes (col, r, "pixbuf", 0, NULL);
  r = gtk_cell_renderer_text_new ();
  gtk_tree_view_column_pack_start (col, r, FALSE);
  gtk_tree_view_column_set_attributes (col, r, "text", 1, NULL);
  gtk_tree_view_column_set_resizable (col, TRUE);
  gtk_tree_view_column_set_expand (col, TRUE);
  gtk_tree_view_append_column (GTK_TREE_VIEW (data->icon_list), col);

  gtk_widget_show_all (data->win);

  /* add callbacks for category and icon lists */
  sel = gtk_tree_view_get_selection (GTK_TREE_VIEW (data->cat_list));
  g_signal_connect (G_OBJECT (sel), "changed", G_CALLBACK (select_cat), data);
  sel = gtk_tree_view_get_selection (GTK_TREE_VIEW (data->icon_list));
  g_signal_connect (G_OBJECT (sel), "changed", G_CALLBACK (select_icon), data);

  /* run it */
  gtk_main ();

  return 0;
}
