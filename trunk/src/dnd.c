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
 * Copyright (C) 2008-2011, Victor Ananjevsky <ananasik@gmail.com>
 *
 */

/* code for working with dnd targets gets from gnome-terminal */

#include "yad.h"

enum {
  TARGET_MOZ_URL,
  TARGET_NS_URL
};

static GtkTargetEntry tgt[] = {
  { "text/x-moz-url",  0, TARGET_MOZ_URL },
  { "_NETSCAPE_URL", 0, TARGET_NS_URL }
};

static void
drop_data_cb (GtkWidget *w, GdkDragContext *dc, gint x, gint y,
	      GtkSelectionData *sel, guint info, guint t, gpointer data)
{
  gchar *str = NULL;
  const guchar *sdata;
  GdkAtom stgt;
  gint slen, sfmt;

  sdata = gtk_selection_data_get_data (sel);
  stgt = gtk_selection_data_get_target (sel);
  slen = gtk_selection_data_get_length (sel);
  sfmt = gtk_selection_data_get_format (sel);

  if (gtk_targets_include_uri (&stgt, 1))
    {
      gchar **uris;
      gint i = 0;
      
      uris = gtk_selection_data_get_uris (sel);
      if (!uris)
	return;

      while (uris[i])
	{
	  gchar *dstr = g_uri_unescape_string (uris[i], NULL);
	  if (options.common_data.command)
	    {
	      gchar *action = g_strdup_printf ("%s '%s'", options.common_data.command, dstr);
	      g_spawn_command_line_async (action, NULL);
	      g_free (action);
	    }
	  else
	    {
	      g_printf ("%s\n", dstr);
	      fflush (stdout);
	    }
	  g_free (dstr);
	  i++;
	}
      g_strfreev (uris);
    }
  else if (gtk_targets_include_text (&stgt, 1))
    {
      gchar *str, *dstr;
      
      str = gtk_selection_data_get_text (sel);
      if (str)
	{
	  dstr = g_uri_unescape_string (str, NULL);
	  if (options.common_data.command)
	    {
	      gchar *action = g_strdup_printf ("%s '%s'", options.common_data.command, dstr);
	      g_spawn_command_line_async (action, NULL);
	      g_free (action);
	    }
	  else
	    {
	      g_printf ("%s\n", dstr);
	      fflush (stdout);
	    }
	  g_free (dstr);
	  g_free(str);
	}
    }
  else
    {
      gchar *str = NULL;

      switch (info)
	{
	case TARGET_MOZ_URL:
	  {
	    gchar *nl;
	    gsize len;

	    /* MOZ_URL is in UCS-2 but in format 8. BROKEN!
	     *
	     * The data contains the URL, a \n, then the title of the web page.
	     */
	    if (sfmt != 8 || slen == 0 || (slen % 2) != 0)
	      return;

	    if ((str = g_utf16_to_utf8 ((const gunichar2*) sdata, slen / 2, 
					NULL, NULL, NULL)) == NULL)
	      return;

	    nl = strchr (str, '\n');
	    if (nl) *nl = '\0';

	    break;
	  }
	case TARGET_NS_URL:
	  {
	    gchar *nl;
	    gsize len;

	    /* The data contains the URL, a \n, then the
	     * title of the web page.
	     */
	    if (slen < 0 || sfmt != 8)
	      return;

	    str = g_strndup ((char *) sdata, slen);
	    nl = strchr (str, '\n');
	    if (nl) *nl = '\0';

	    break;
	  }
	}

      if (str)
	{
	  gchar *dstr = g_uri_unescape_string (str, NULL);
	  if (options.common_data.command)
	    {
	      gchar *action = g_strdup_printf ("%s '%s'", options.common_data.command, dstr);
	      g_spawn_command_line_async (action, NULL);
	      g_free (action);
	    }
	  else
	    {
	      g_printf ("%s\n", dstr);
	      fflush (stdout);
	    }
	  g_free (dstr);
	  g_free (str);
	}
    }
}

void
dnd_init (GtkWidget *w)
{
  GtkTargetList *tlist;
  GtkTargetEntry *tgts;
  gint ntgts;

  tlist = gtk_target_list_new (NULL, 0);
  gtk_target_list_add_uri_targets (tlist, 0);
  gtk_target_list_add_text_targets (tlist, 0);
  gtk_target_list_add_table (tlist, tgt, G_N_ELEMENTS (tgt));

  tgts = gtk_target_table_new_from_list (tlist, &ntgts);

  gtk_drag_dest_set (w, GTK_DEST_DEFAULT_ALL, tgts, ntgts, 
		     GDK_ACTION_COPY | GDK_ACTION_MOVE);
  g_signal_connect (G_OBJECT (w), "drag_data_received",
                    G_CALLBACK (drop_data_cb), NULL);

  gtk_target_table_free (tgts, ntgts);
  gtk_target_list_unref (tlist);
}
