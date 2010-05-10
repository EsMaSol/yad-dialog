
#include "yad.h"

static GtkWidget *icon_view;

enum {
  COL_PATH,
  COL_DISPLAY_NAME,
  COL_PIXBUF,
  COL_COMMAND,
  NUM_COLS
};

static gboolean
handle_stdin (GIOChannel * channel,
              GIOCondition condition, gpointer data)
{
}

static void
read_dir (GtkListStore *store)
{
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
			      G_TYPE_STRING);

  icon_view = gtk_icon_view_new_with_model (GTK_TREE_MODEL (store));

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

  gtk_container_add (GTK_CONTAINER (w), icon_view);

  return w;
}
