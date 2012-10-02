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
 * along with YAD. If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) 2008-2012, Victor Ananjevsky <ananasik@gmail.com>
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>

#include "yad.h"

static GtkWidget *notebook;

static guint *plugs;

static GHashTable *wins = NULL;

static void
create_wins_hash (void)
{
  Atom type;
  gint format;
  gulong i, nitems, bytes_after;
  GList *data, *tmp;
  gint res;

  wins = g_hash_table_new (g_str_hash, g_str_equal);

  /* get window list */
  data = gdk_window_get_children (gdk_get_default_root_window ());
  //printf ("%d\n", g_list_length (data));

  for (tmp = data; tmp; tmp = tmp->next)
    {
      XClassHint ch;
      GdkWindow *w = (GdkWindow *) tmp->data;

      //printf ("0x%X\n", gdk_x11_drawable_get_xid (GDK_DRAWABLE (w)));
      res = XGetClassHint (gdk_x11_get_default_xdisplay (), 
			   gdk_x11_drawable_get_xid (GDK_DRAWABLE (w)), &ch);

      //printf ("%s - %s\n", ch.res_class, ch.res_name);
      if (res != Success)
	continue;
      if (strcmp (ch.res_class, "YAD-PLUG") != 0)
	continue;
      
      //printf ("get %s\n", ch.res_name);
    }
}

GtkWidget *
notebook_create_widget (GtkWidget *dlg)
{
  GtkWidget *w;
  GSList *tab;

  w = notebook = gtk_notebook_new ();
  gtk_container_set_border_width (GTK_CONTAINER (w), 5);

  /* add tabs */
  for (tab = options.notebook_data.tabs; tab; tab = tab->next)
    {
      GtkWidget *l, *s, *e;

      l = gtk_label_new (NULL);
      if (!options.data.no_markup)
	gtk_label_set_markup (GTK_LABEL (l), (gchar *) tab->data);
      else
	gtk_label_set_text (GTK_LABEL (l), (gchar *) tab->data);
      gtk_misc_set_alignment (GTK_MISC (l), options.common_data.align, 0.5);

      e = gtk_event_box_new ();
      gtk_container_set_border_width (GTK_CONTAINER (e), options.notebook_data.borders);

      s = gtk_socket_new ();
      gtk_container_add (GTK_CONTAINER (e), s);

      gtk_notebook_append_page (GTK_NOTEBOOK (w), e, l);
    }

  return w;
}

void
notebook_swallow_childs (void)
{
  plugs = g_new0 (guint, g_slist_length (options.notebook_data.tabs));

  if (options.extra_data)
    {
      gint i = 0;
      
      create_wins_hash ();

      while (options.extra_data[i])
	{
	  guint id = 0;

	  /* get client id */
	  id = strtoul (options.extra_data[i], NULL, 0);
	  
	  if (id)
	    {
	      /* data is XID */
	      GtkWidget *e = gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), i);
	      if (e)
		{
		  GList *cl;

		  /* add plug to socket */
		  cl = gtk_container_get_children (GTK_CONTAINER (e));
		  gtk_socket_add_id (GTK_SOCKET (cl->data), (GdkNativeWindow) id);
		}
	    }
	  else
	    {
	      /* data is YAD's plug */
	    }

	  i++;
	}

      g_hash_table_destroy (wins);
    }
}

void
notebook_print_result (void)
{
  guint i, n_tabs;

  n_tabs = g_slist_length (options.notebook_data.tabs);
  for (i = 0; i < n_tabs; i++)
    {
      if (plugs[i])
	{
	}
    }
}

void
notebook_close_childs (void)
{
}
