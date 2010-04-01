
#include <string.h>

#include "yad.h"

static GtkWidget *filechooser;

static void
file_activated_cb (GtkFileChooser *chooser, gpointer *data)
{
  gtk_dialog_response (GTK_DIALOG (data), YAD_RESPONSE_OK);
}

static GtkFileChooserConfirmation
confirm_overwrite_cb (GtkFileChooser *chooser, gpointer data)
{
  return GTK_FILE_CHOOSER_CONFIRMATION_CONFIRM;
}

GtkWidget * 
file_create_widget (GtkWidget *dlg)
{
  GtkWidget *w;
  gchar *dir, *basename;
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;

  if (options.file_data.directory)
    {
      if (options.file_data.save)
        action = GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER;
      else
        action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
    }
  else
    {
      if (options.file_data.save)
        action = GTK_FILE_CHOOSER_ACTION_SAVE;
    }

  w = filechooser = gtk_file_chooser_widget_new (action);
  g_signal_connect (w, "file-activated",
		    G_CALLBACK (file_activated_cb), dlg);

  if (options.file_data.confirm_overwrite)
    {
      gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (w), TRUE);
      g_signal_connect (w, "confirm-overwrite", 
			G_CALLBACK (confirm_overwrite_cb), NULL);
    }

  if (options.common_data.uri)
    {
      dir = g_path_get_dirname (options.common_data.uri);

      if (g_path_is_absolute (options.common_data.uri) == TRUE)
        gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (w), dir);

      if (options.common_data.uri[strlen (options.common_data.uri) - 1] != '/')
        {
          basename = g_path_get_basename (options.common_data.uri);
          if (options.file_data.save)
            gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (w),
                                               basename);
          else
            gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (w),
					   options.common_data.uri);
          g_free (basename);
        }
      g_free (dir);
    }

  if (options.common_data.multi)
    gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER (w), TRUE);

  if (options.file_data.filter)
    {
      /* Filter format: Executables | *.exe *.bat *.com */
      gint filter_i;

      for (filter_i = 0; options.file_data.filter[filter_i]; filter_i++)
        {
          GtkFileFilter *filter = gtk_file_filter_new ();
          gchar *filter_str = options.file_data.filter[filter_i];
          gchar **pattern, **patterns;
          gchar *name = NULL;
          gint i;

          /* Set name */
          for (i = 0; filter_str[i] != '\0'; i++)
            if (filter_str[i] == '|')
              break;

          if (filter_str[i] == '|')
            {
              name = g_strndup (filter_str, i);
              g_strstrip (name);
            }

          if (name)
            {
              gtk_file_filter_set_name (filter, name);

              /* Point i to the right position for split */
              for (++i; filter_str[i] == ' '; i++);
            }
          else
            {
              gtk_file_filter_set_name (filter, filter_str);
              i = 0;
            }

          /* Get patterns */
          patterns = g_strsplit_set (filter_str + i, " ", -1);

          for (pattern = patterns; *pattern; pattern++)
            gtk_file_filter_add_pattern (filter, *pattern);

	  g_free (name);
          g_strfreev (patterns);

          gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (w), filter);
        }
    }
  
  return w;
}

void
file_print_result (void)
{
  GSList *selections, *iter;

  selections = gtk_file_chooser_get_filenames (GTK_FILE_CHOOSER (filechooser));
  for (iter = selections; iter != NULL; iter = iter->next)
    {
      g_print ("%s",
	       g_filename_to_utf8 ((gchar *) iter->data, -1, NULL, NULL,
				   NULL));
      g_free (iter->data);
      if (iter->next != NULL)
	g_print ("%s", options.common_data.separator);
    }
  g_print ("\n");
  g_slist_free (selections);
}
