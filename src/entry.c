
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

  if (!options.entry_data.completion && 
      options.extra_data && *options.extra_data)
    {
      gint i = 0;

      if (options.common_data.editable || settings.combo_always_editable)
	{
	  c = gtk_combo_box_entry_new_text ();
	  entry = gtk_bin_get_child (GTK_BIN (c));
	}
      else
	{
	  c = entry = gtk_combo_box_new_text ();
	  is_combo = TRUE;
	}

      while (options.extra_data[i] != NULL)
	{
	  gtk_combo_box_append_text (GTK_COMBO_BOX (c), options.extra_data[i]);
	  i++;
	}

      if (options.entry_data.entry_text)
  	{
  	  gtk_combo_box_prepend_text (GTK_COMBO_BOX (c),
  				      options.entry_data.entry_text);
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
    g_signal_connect (entry, "activate", G_CALLBACK (entry_activate_cb), dlg);

  gtk_box_pack_start (GTK_BOX (w), c, TRUE, TRUE, 1);

  return w;
}

void
entry_print_result (void)
{
  if (is_combo)
    g_print ("%s\n", gtk_combo_box_get_active_text (GTK_COMBO_BOX (entry)));
  else
    g_print ("%s\n", gtk_entry_get_text (GTK_ENTRY (entry)));
}
