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

#include "yad.h"

static GtkWidget *entry;
static is_combo = FALSE;

static void
entry_activate_cb (GtkEntry *entry, gpointer data)
{
  gtk_dialog_response (GTK_DIALOG (data), YAD_RESPONSE_OK);
}

static GtkTreeModel *
create_completion_model (void)
{
  GtkListStore *store;
  GtkTreeIter iter;
  gint i = 0;

  store = gtk_list_store_new (1, G_TYPE_STRING);

  if (options.extra_data)
    {
      while (options.extra_data[i] != NULL)
	{
	  gtk_list_store_append (store, &iter);
	  gtk_list_store_set (store, &iter, 0, options.extra_data[i], -1);
	  i++;
	}
    }

  return GTK_TREE_MODEL (store);
}

GtkWidget *
entry_create_widget (GtkWidget *dlg)
{
  GtkWidget *c, *w = NULL;

  w = gtk_hbox_new (FALSE, 5);

  if (options.entry_data.entry_label)
    {
      GtkWidget *l = gtk_label_new (options.entry_data.entry_label);
      gtk_box_pack_start (GTK_BOX (w), l, FALSE, FALSE, 1);
    }

  if (options.entry_data.numeric)
    {
      gdouble min, max, step, val;

      min = 0.0; max = 65535.0; step = 1.0;

      if (options.extra_data && *options.extra_data)
	{
	  min = g_ascii_strtod (options.extra_data[0], NULL);
	  if (options.extra_data[1])
	    {
	      max = g_ascii_strtod (options.extra_data[1], NULL);
	      if (options.extra_data[2])
		step = g_ascii_strtod (options.extra_data[2], NULL);
	    }
	}

      c = entry = gtk_spin_button_new_with_range (min, max, step);

      if (options.entry_data.entry_text)
	{
	  val = g_ascii_strtod (options.entry_data.entry_text, NULL);

	  if (min >= max)
	    {
	      g_printerr (_("Maximum value must be greater than minimum value.\n"));
	      min = 0.0; max = 65535.0;
	    }

 	  if (val < min)
	    {
	      g_printerr (_("Initial value less than minimal.\n"));
	      val = min;
	    }
	  else if (val > max)
	    {
	      g_printerr (_("Initial value greater than maximum.\n"));
	      val = max;
	    }

	  gtk_spin_button_set_value (GTK_SPIN_BUTTON (c), val);
	}
    }
  else if (!options.entry_data.completion &&
      options.extra_data && *options.extra_data)
    {
      gint i = 0;

      if (options.common_data.editable || settings.combo_always_editable)
	{
#if GTK_CHECK_VERSION(2,24,0)
	  c = gtk_combo_box_text_new_with_entry ();
#else
	  c = gtk_combo_box_entry_new_text ();
#endif
	  entry = gtk_bin_get_child (GTK_BIN (c));
	}
      else
	{
#if GTK_CHECK_VERSION(2,24,0)
	  c = entry = gtk_combo_box_text_new ();
#else
	  c = entry = gtk_combo_box_new_text ();
#endif
	  is_combo = TRUE;
	}

      while (options.extra_data[i] != NULL)
	{
#if GTK_CHECK_VERSION(2,24,0)
	  gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (c), options.extra_data[i]);
#else
	  gtk_combo_box_append_text (GTK_COMBO_BOX (c), options.extra_data[i]);
#endif
	  i++;
	}

      if (options.entry_data.entry_text)
  	{
#if GTK_CHECK_VERSION(2,24,0)
  	  gtk_combo_box_text_prepend_text (GTK_COMBO_BOX_TEXT (c),
  				      options.entry_data.entry_text);
#else
  	  gtk_combo_box_prepend_text (GTK_COMBO_BOX (c),
  				      options.entry_data.entry_text);
#endif
  	  gtk_combo_box_set_active (GTK_COMBO_BOX (c), 0);
  	}
    }
  else
    {
      c = entry = gtk_entry_new ();

      gtk_entry_set_activates_default (GTK_ENTRY (entry), TRUE);

      if (options.entry_data.entry_text)
  	gtk_entry_set_text (GTK_ENTRY (entry), options.entry_data.entry_text);

      if (options.entry_data.hide_text)
  	g_object_set (G_OBJECT (entry), "visibility", FALSE, NULL);

      if (options.entry_data.completion)
	{
	  GtkEntryCompletion *completion;
	  GtkTreeModel *completion_model;

	  completion = gtk_entry_completion_new ();
	  gtk_entry_set_completion (GTK_ENTRY (entry), completion);
	  g_object_unref (completion);

	  completion_model = create_completion_model ();
	  gtk_entry_completion_set_model (completion, completion_model);
	  g_object_unref (completion_model);

	  gtk_entry_completion_set_text_column (completion, 0);
	}
    }
  if (!is_combo)
    g_signal_connect (G_OBJECT (entry), "activate",
		      G_CALLBACK (entry_activate_cb), dlg);

  gtk_box_pack_start (GTK_BOX (w), c, TRUE, TRUE, 1);

  return w;
}

void
entry_print_result (void)
{
  if (options.entry_data.numeric)
    g_print ("%lf\n", gtk_spin_button_get_value (GTK_SPIN_BUTTON (entry)));
  else if (is_combo)
#if GTK_CHECK_VERSION(2,24,0)
    g_print ("%s\n", gtk_combo_box_text_get_active_text (GTK_COMBO_BOX_TEXT (entry)));
#else
    g_print ("%s\n", gtk_combo_box_get_active_text (GTK_COMBO_BOX (entry)));
#endif
  else
    g_print ("%s\n", gtk_entry_get_text (GTK_ENTRY (entry)));
}
