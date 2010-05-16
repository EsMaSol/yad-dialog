
#include "yad.h"

static GtkWidget *icon_view;

enum {
  COL_NAME,
  COL_TOOLTIP,
  COL_PIXBUF,
  COL_COMMAND,
  COL_TERM,
  NUM_COLS
};

typedef struct {
  gchar *name;
  gchar *comment;
  GdkPixbuf *pixbuf;
  gchar *command;
  gboolean in_term;
} DEntry;

static void
activate_cb (GtkIconView *view, GtkTreePath *path, gpointer data)
{
  GtkTreeIter iter;
  GtkTreeModel *model = gtk_icon_view_get_model (view);
  gchar *cmd;
  gboolean *in_term;

  gtk_tree_model_get_iter (model, &iter, path);
  gtk_tree_model_get (model, &iter,
		      COL_COMMAND, &cmd,
		      COL_TERM, &in_term,
		      -1);

  if (in_term)
    {
      gchar *tcmd;
      
      tcmd = g_strdup_printf (options.icons_data.term, cmd);
      g_spawn_command_line_async (tcmd, NULL);
      g_free (tcmd);
    }
  else
    g_spawn_command_line_async (cmd, NULL);
}

static gboolean
handle_stdin (GIOChannel * channel,
              GIOCondition condition, gpointer data)
{
}

static DEntry *
parse_desktop_file (gchar *filename)
{
  DEntry *ent;
  GKeyFile *kf;
  GError *err = NULL;
  static GdkPixbuf *fb = NULL;

  if (!fb)
    fb = gtk_icon_theme_load_icon (settings.icon_theme, "unknown",
				   48, GTK_ICON_LOOKUP_GENERIC_FALLBACK,
				   NULL);

  ent = g_new0 (DEntry, 1);
  kf = g_key_file_new ();

  if (g_key_file_load_from_file (kf, filename, 0, &err))
    {
      gchar *icon;

      if (g_key_file_has_group (kf, "Desktop Entry"))
	{
	  gint i;

	  ent->name = g_key_file_get_locale_string (kf, "Desktop Entry", "Name", NULL, NULL);
	  ent->comment = g_key_file_get_locale_string (kf, "Desktop Entry", "Comment", NULL, NULL);
	  ent->command = g_key_file_get_string (kf, "Desktop Entry", "Exec", NULL);
	  /* remove possible arguments patterns */
	  for (i = strlen (ent->command); i > 0; i--)
	    {
	      if (ent->command[i] == '%')
		{
		  ent->command[i] = '\0';
		  break;
		}
	    }
	  ent->in_term = g_key_file_get_boolean (kf, "Desktop Entry", "Terminal", NULL);
	  icon = g_key_file_get_string (kf, "Desktop Entry", "Icon", NULL);
	  if (icon)
	    {
	      ent->pixbuf = gtk_icon_theme_load_icon (settings.icon_theme, icon, 48, 
						      GTK_ICON_LOOKUP_GENERIC_FALLBACK, NULL);
	      if (!ent->pixbuf)
		ent->pixbuf = fb;
	      g_free (icon);
	    }
	}
    }
  else
    g_warning (_("Unable to parse file %s: %s"), filename, err->message);
  
  g_key_file_free (kf);

  return ent;
}

static void
read_dir (GtkListStore *store)
{
  GDir *dir;
  const gchar *filename;
  GError *err = NULL;

  dir = g_dir_open (options.icons_data.directory, 0, &err);
  if (!dir)
    {
      g_warning (_("Unable to open directory %s: %s"), 
		 options.icons_data.directory, err->message);
      return;
    }

  while ((filename = g_dir_read_name (dir)) != NULL)
    {
      DEntry *ent;
      GtkTreeIter iter;
      gchar *fullname;

      if (!g_str_has_suffix (filename, ".desktop"))
	continue;

      fullname = g_build_filename (options.icons_data.directory, filename, NULL);
      ent = parse_desktop_file (fullname);
      g_free (fullname);

      if (ent->name)
	{
	  gtk_list_store_append (store, &iter);
	  gtk_list_store_set (store, &iter,
			      COL_NAME, ent->name,
			      COL_TOOLTIP, ent->comment ? ent->comment : "",
			      COL_PIXBUF, ent->pixbuf,
			      COL_COMMAND, ent->command ? ent->command : "",
			      COL_TERM, ent->in_term,
			      -1);
	}

      /* free desktop entry */
      g_free (ent->name);
      g_free (ent->comment);
      g_free (ent->command);
      if (ent->pixbuf)
	g_object_unref (ent->pixbuf);
      g_free (ent);
    }

  g_dir_close (dir);
}

GtkWidget *
icons_create_widget (GtkWidget *dlg)
{
  GtkWidget *w;
  GtkListStore *store;

  w = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (w),
				  GTK_POLICY_AUTOMATIC,
				  GTK_POLICY_AUTOMATIC);

  store = gtk_list_store_new (NUM_COLS,
			      G_TYPE_STRING,
			      G_TYPE_STRING,
			      GDK_TYPE_PIXBUF,
			      G_TYPE_STRING,
			      G_TYPE_BOOLEAN);

  icon_view = gtk_icon_view_new_with_model (GTK_TREE_MODEL (store));
  gtk_icon_view_set_text_column (GTK_ICON_VIEW (icon_view), COL_NAME);
  gtk_icon_view_set_pixbuf_column (GTK_ICON_VIEW (icon_view), COL_PIXBUF);
  gtk_icon_view_set_tooltip_column (GTK_ICON_VIEW (icon_view), COL_TOOLTIP);

  /* handle directory */
  if (options.icons_data.directory)
    read_dir (store);

  g_object_unref (store);

  /* read from stdin */
  if (options.icons_data.stdin)
    {
      GIOChannel *channel;

      channel = g_io_channel_unix_new (0);
      g_io_channel_set_encoding (channel, NULL, NULL);
      g_io_channel_set_flags (channel, G_IO_FLAG_NONBLOCK, NULL);
      g_io_add_watch (channel, G_IO_IN | G_IO_HUP, handle_stdin, NULL);
    }

  g_signal_connect (G_OBJECT (icon_view), "item-activated",
		    G_CALLBACK (activate_cb), NULL);
  gtk_container_add (GTK_CONTAINER (w), icon_view);

  return w;
}
