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

#include "yad.h"

static GtkWidget *notebook;

GIOChannel **plugs;

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
  plugs = g_new0 (GIOChannel*, g_slist_length (options.notebook_data.tabs));

  if (options.extra_data)
    {
      gint i = 0;
      while (options.extra_data[i])
	{
	  guint id = 0;

	  /* get client id */
	  if (g_file_test (options.extra_data[i], G_FILE_TEST_EXISTS))
	    {
	      gint fd = open (options.extra_data[i], O_RDWR);
	  
	      if (fd != -1)
	        {
	          plugs[i] = g_io_channel_unix_new (fd);
	          g_io_channel_set_encoding (plugs[i], NULL, NULL);
	          g_io_channel_set_flags (plugs[i], G_IO_FLAG_NONBLOCK, NULL);
	        }
              if (g_io_channel_write_chars (plugs[i], "id\n", -1, NULL, NULL) == G_IO_STATUS_NORMAL)
                {
                  gchar *buf;
                  if (g_io_channel_read_line (plugs[i], &buf, NULL, NULL, NULL) == G_IO_STATUS_NORMAL)
                    {
                      id = strtoul (buf, NULL, 0);
                      g_free (buf);
                    }
                }
	    }
          else
	    id = strtoul (options.extra_data[i], NULL, 0);
	  
	  /* add id to socket */
	  if (id)
	    {
	      GtkWidget *e = gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), i);
	      if (e)
		{
		  GList *cl;

		  /* add plug to socket */
		  cl = gtk_container_get_children (GTK_CONTAINER (e));
		  gtk_socket_add_id (GTK_SOCKET (cl->data), (GdkNativeWindow) id);
		}
	    }
	  i++;
	}
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
	  if (g_io_channel_write_chars (plugs[i], "print\n", -1, NULL, NULL) == G_IO_STATUS_NORMAL)
	    {
	      gchar *buf;
	      while (g_io_channel_read_line (plugs[i], &buf, NULL, NULL, NULL) == G_IO_STATUS_NORMAL)
		{
		  g_print ("%s", buf);
		  g_free (buf);
		}
	    }
	  g_io_channel_write_chars (plugs[i], "quit\n", -1, NULL, NULL);
	}
    }
}
