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
 * Copyright (C) 2008-2015, Victor Ananjevsky <ananasik@gmail.com>
 */

#include "yad.h"

#include <webkit/webkit.h>

static WebKitWebView *view;
static gboolean is_link;

static void
load_uri (const gchar *uri)
{
  gchar *addr = NULL;

  if (!uri || !uri[0])
    return;

  if (g_file_test (uri, G_FILE_TEST_EXISTS))
    addr = g_filename_to_uri (uri, NULL, NULL);
  else
    {
      if (g_uri_parse_scheme (uri) == NULL)
        addr = g_strdup_printf ("http://%s", uri);
      else
        addr = g_strdup (uri);
    }

  webkit_web_view_load_uri (view, addr);
  g_free (addr);
}

static void
load_stdin_done (GObject *obj, GAsyncResult *res, gpointer d)
{
  webkit_web_view_load_string (view, (gchar *) g_async_result_get_user_data (res), NULL, NULL, NULL);
}

static gboolean
link_cb (WebKitWebView *v, WebKitWebFrame *f, WebKitNetworkRequest *r,
         WebKitWebNavigationAction *act, WebKitWebPolicyDecision *pd, gpointer d)
{
  return TRUE;
}

static void
link_hover_cb (WebKitWebView *view, const gchar *title, const gchar *link, GtkStatusbar *sb)
{
  if (link)
    is_link = TRUE;
  else
    is_link = FALSE;
}

static void
select_file_cb (GtkEntry *entry, GtkEntryIconPosition pos, GdkEventButton *ev, gpointer d)
{
  GtkWidget *dlg;
  static gchar *dir = NULL;

  if (ev->button != 1 || pos != GTK_ENTRY_ICON_SECONDARY)
    return;

  dlg = gtk_file_chooser_dialog_new (_("IxHTML - Select File"),
                                     GTK_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET (entry))),
                                     GTK_FILE_CHOOSER_ACTION_OPEN,
                                     GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
                                     GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                     NULL);
  if (dir)
    gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dlg), dir);

  if (gtk_dialog_run (GTK_DIALOG (dlg)) == GTK_RESPONSE_ACCEPT)
    {
      gchar *uri = gtk_file_chooser_get_uri (GTK_FILE_CHOOSER (dlg));
      gtk_entry_set_text (entry, uri);
      g_free (uri);

      g_free (dir);
      dir = gtk_file_chooser_get_current_folder (GTK_FILE_CHOOSER (dlg));
    }

 gtk_widget_destroy (dlg);
}

static void
do_open_cb (GtkWidget *w, GtkDialog *dlg)
{
  gtk_dialog_response (dlg, GTK_RESPONSE_ACCEPT);
}

static void
open_cb (GtkWidget *w, gpointer d)
{
  GtkWidget *dlg, *cnt, *lbl, *entry;

  dlg = gtk_dialog_new_with_buttons (_("Open URI"),
                                     GTK_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET (view))),
                                     GTK_DIALOG_DESTROY_WITH_PARENT,
                                     GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
                                     GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                     NULL);
  gtk_window_set_default_size (GTK_WINDOW (dlg), 350, -1);

  cnt = gtk_dialog_get_content_area (GTK_DIALOG (dlg));

  lbl = gtk_label_new (_("Enter URI or file name:"));
  gtk_misc_set_alignment (GTK_MISC (lbl), 0, 0);
  gtk_widget_show (lbl);
  gtk_box_pack_start (GTK_BOX (cnt), lbl, TRUE, FALSE, 2);

  entry = gtk_entry_new ();
  gtk_entry_set_icon_from_stock (GTK_ENTRY (entry), GTK_ENTRY_ICON_SECONDARY, "gtk-directory");
  gtk_widget_show (entry);
  gtk_box_pack_start (GTK_BOX (cnt), entry, TRUE, FALSE, 2);

  g_signal_connect (G_OBJECT (entry), "icon-press", G_CALLBACK (select_file_cb), NULL);
  g_signal_connect (G_OBJECT (entry), "activate", G_CALLBACK (do_open_cb), dlg);

  if (gtk_dialog_run (GTK_DIALOG (dlg)) == GTK_RESPONSE_ACCEPT)
    load_uri (gtk_entry_get_text (GTK_ENTRY (entry)));

  gtk_widget_destroy (dlg);
}

static gboolean
menu_cb (WebKitWebView *view, GtkWidget *menu, WebKitHitTestResult *hit,
         gboolean kb, gpointer d)
{
  GtkWidget *mi;

  if (!is_link)
    {
      mi = gtk_separator_menu_item_new ();
      gtk_widget_show (mi);
      gtk_menu_shell_prepend (GTK_MENU_SHELL (menu), mi);

      mi = gtk_image_menu_item_new_from_stock ("gtk-open", NULL);
      gtk_widget_show (mi);
      gtk_menu_shell_prepend (GTK_MENU_SHELL (menu), mi);
      g_signal_connect (G_OBJECT (mi), "activate", G_CALLBACK (open_cb), NULL);

    }

  return FALSE;
}

static gboolean
ready_cb (WebKitWebView *v, gpointer d)
{
  gtk_widget_grab_focus (GTK_WIDGET (v));
  return FALSE;
}

GtkWidget *
html_create_widget (GtkWidget *dlg)
{
  GtkWidget *sw;

  sw = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  view = WEBKIT_WEB_VIEW (webkit_web_view_new ());
  gtk_container_add (GTK_CONTAINER (sw), GTK_WIDGET (view));

  g_signal_connect (view, "hovering-over-link", G_CALLBACK (link_hover_cb), NULL);
  g_signal_connect (view, "web-view-ready", G_CALLBACK (ready_cb), NULL);

  if (options.html_data.browser)
    g_signal_connect (view, "context-menu", G_CALLBACK (menu_cb), NULL);
  else
    g_signal_connect (view, "navigation-policy-decision-requested", G_CALLBACK (link_cb), NULL);

  soup_session_add_feature_by_type (webkit_get_default_session (), SOUP_TYPE_PROXY_RESOLVER_DEFAULT);

  gtk_widget_show_all (sw);

  if (options.html_data.uri)
    load_uri (options.html_data.uri);
  else
    {
      GInputStream *stream = (GInputStream *) g_unix_input_stream_new (0, FALSE);
      g_input_stream_read_async (stream, g_new0 (gchar, G_MAXSSIZE), G_MAXSSIZE, G_PRIORITY_DEFAULT,
                                 NULL, load_stdin_done, NULL);
      g_object_unref (stream);
    }

  return sw;
}
