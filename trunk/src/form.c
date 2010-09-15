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

	  switch (fld->type)
	    {
	    case YAD_FIELD_SIMPLE:
	    case YAD_FIELD_HIDDEN:
	    case YAD_FIELD_READ_ONLY:
	      l = gtk_label_new (fld->name);
	      gtk_misc_set_alignment (GTK_MISC (l), 0.0, 1.0);
	      gtk_table_attach (GTK_TABLE (w), l, 0, 1, i, i + 1, 0, 0, 5, 5);

	      e = gtk_entry_new ();
	      g_signal_connect (G_OBJECT (e), "activate", G_CALLBACK (form_activate_cb), dlg);
	      if (fld->type == YAD_FIELD_HIDDEN)
		gtk_entry_set_visibility (GTK_ENTRY (e), FALSE);
	      else if (fld->type == YAD_FIELD_READ_ONLY)
		gtk_entry_set_editable (GTK_ENTRY (e), FALSE);
	      gtk_table_attach (GTK_TABLE (w), e, 1, 2, i, i + 1, GTK_EXPAND | GTK_FILL, 0, 5, 5);
	      fields = g_slist_append (fields, e);
	      break;

	    case YAD_FIELD_NUM:
	      l = gtk_label_new (fld->name);
	      gtk_misc_set_alignment (GTK_MISC (l), 0.0, 1.0);
	      gtk_table_attach (GTK_TABLE (w), l, 0, 1, i, i + 1, 0, 0, 5, 5);

	      e = gtk_spin_button_new_with_range (0.0, 65525.0, 1.0);
	      gtk_table_attach (GTK_TABLE (w), e, 1, 2, i, i + 1, GTK_EXPAND | GTK_FILL, 0, 5, 5);
	      fields = g_slist_append (fields, e);	      
	      break;
	      
	    case YAD_FIELD_CHECK:
	      e = gtk_check_button_new_with_label (fld->name);
	      gtk_table_attach (GTK_TABLE (w), e, 0, 2, i, i + 1, GTK_EXPAND | GTK_FILL, 0, 5, 5);
	      fields = g_slist_append (fields, e);	      
	      break;
	      
	    case YAD_FIELD_COMBO:
	      l = gtk_label_new (fld->name);
	      gtk_misc_set_alignment (GTK_MISC (l), 0.0, 1.0);
	      gtk_table_attach (GTK_TABLE (w), l, 0, 1, i, i + 1, 0, 0, 5, 5);

	      e = gtk_combo_box_new_text ();
	      gtk_table_attach (GTK_TABLE (w), e, 1, 2, i, i + 1, GTK_EXPAND | GTK_FILL, 0, 5, 5);
	      fields = g_slist_append (fields, e);
	      break;
	    }
	}

      /* fill entries with data */
      if (options.extra_data)
	{
	  i = 0;
	  while (options.extra_data[i] && i < fc)
	    {
	      YadField *fld = g_slist_nth_data (options.form_data.fields, i);

	      switch (fld->type)
		{
		case YAD_FIELD_SIMPLE:
		case YAD_FIELD_HIDDEN:
		case YAD_FIELD_READ_ONLY:
		  gtk_entry_set_text (GTK_ENTRY (g_slist_nth_data (fields, i)), options.extra_data[i]);
		  break;
		case YAD_FIELD_NUM:
		  break;
		case YAD_FIELD_CHECK:
		  if (g_ascii_strcasecmp (options.extra_data[i], "TRUE") == 0)
		    {
		      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (g_slist_nth_data (fields, i)),
						    TRUE);
		    }
		  break;
		case YAD_FIELD_COMBO:
		  break;	  
		}
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
      YadField *fld = g_slist_nth_data (options.form_data.fields, i);
      
      switch (fld->type)
	{
	case YAD_FIELD_SIMPLE:
	case YAD_FIELD_HIDDEN:
	case YAD_FIELD_READ_ONLY:
	  g_printf ("%s%s",
		    gtk_entry_get_text (GTK_ENTRY (g_slist_nth_data (fields, i))),
		    options.common_data.separator);
	  break;
	case YAD_FIELD_NUM:
	  g_printf ("%f%s",
		    gtk_spin_button_get_value (GTK_SPI_BUTTON (g_slist_nth_data (fields, i))),
		    options.common_data.separator);
	  break;
	case YAD_FIELD_CHECK:
	  g_printf ("%s%s",
		    gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (fields, i)) ? "TRUE" : "FALSE",
		    options.common_data.separator);
	  break;
	case YAD_FIELD_COMBO:
	  g_printf ("%s%s",
		    gtk_combo_box_get_active_text (GTK_COMBO_BOX (g_slist_nth_data (fields, i))),
		    options.common_data.separator);
	  break;	  
	}
    }
  g_printf ("\n");
}
