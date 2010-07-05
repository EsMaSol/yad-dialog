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

#include "yad.h"

GSList *fields = NULL;

static void
form_activate_cb (GtkEntry *entry, gpointer data)
{
  gtk_dialog_response (GTK_DIALOG (data), YAD_RESPONSE_OK);
}

GtkWidget *
form_create_widget (GtkWidget *dlg)
{
  GtkWidget *w = NULL;

  if (options.form_data.fields)
    {
      GtkWidget *l, *e;
      guint i, fc = g_slist_length (options.form_data.fields);

      w = gtk_table_new (fc, 2, FALSE);

      /* create form */
      for (i = 0; i < fc; i++)
	{
	  YadField *fld = g_slist_nth_data (options.form_data.fields, i);

	  l = gtk_label_new (fld->name);
	  gtk_misc_set_alignment (GTK_MISC (l), 0.0, 1.0);
	  gtk_table_attach (GTK_TABLE (w), l, 0, 1, i, i + 1, 0, 0, 5, 5);
	  
	  e = gtk_entry_new ();
	  g_signal_connect (G_OBJECT (e), "activate", G_CALLBACK (form_activate_cb), dlg);
	  if (fld->type == YAD_FIELD_HIDDEN)
	    gtk_entry_set_visibility (GTK_ENTRY (e), FALSE);
	  if (fld->type == YAD_FIELD_READ_ONLY)
	    gtk_entry_set_editable (GTK_ENTRY (e), FALSE);
	  gtk_table_attach (GTK_TABLE (w), e, 1, 2, i, i + 1, GTK_EXPAND | GTK_FILL, 0, 5, 5);
	  fields = g_slist_append (fields, e);
	}

      /* fill entries with data */
      if (options.extra_data)
	{
	  i = 0;
	  while (options.extra_data[i] && i < fc)
	    {
	      gtk_entry_set_text (GTK_ENTRY (g_slist_nth_data (fields, i)), options.extra_data[i]);
	      i++;
	    }
	}
    }

  return w;
}

void
form_print_result (void)
{
  guint i;

  for (i = 0; i < g_slist_length (fields); i++)
    {
      g_printf ("%s%s", 
		gtk_entry_get_text (GTK_ENTRY (g_slist_nth_data (fields, i))), 
		options.common_data.separator);
    }
  g_printf ("\n");
}
