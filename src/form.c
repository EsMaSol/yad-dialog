
#include "yad.h"

GSList *fields = NULL;

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
	  l = gtk_label_new (g_slist_nth_data (options.form_data.fields, i));
	  gtk_misc_set_alignment (GTK_MISC (l), 0.0, 1.0);
	  gtk_table_attach (GTK_TABLE (w), l, 0, 1, i, i + 1, 0, 0, 5, 5);
	  
	  e = gtk_entry_new ();
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
