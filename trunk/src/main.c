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

#include <locale.h>
#include <stdlib.h>
#include <signal.h>

#include "yad.h"

YadOptions options;
GtkWidget *dialog = NULL;

static void
sa_usr1 (gint sig)
{
  gtk_dialog_response (GTK_DIALOG (dialog), YAD_RESPONSE_OK);
}

static void
sa_usr2 (gint sig)
{
  gtk_dialog_response (GTK_DIALOG (dialog), YAD_RESPONSE_CANCEL);
}

static gboolean
timeout_cb (gpointer data)
{
  GtkWidget *w = (GtkWidget *) data;

  gtk_dialog_response (GTK_DIALOG (w), YAD_RESPONSE_TIMEOUT);

  return FALSE;
}

static gboolean
timeout_indicator_cb (gpointer data)
{
  static guint count = 1;
  gdouble percent;
  GtkWidget *w = (GtkWidget *) data;

  if (!w)
    return FALSE;

  percent =
    ((gdouble) options.data.timeout - count) / (gdouble) options.data.timeout;
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (w), percent);
  if (settings.show_remain)
    {
      gchar *lbl =
        g_strdup_printf (_("%d sec"), options.data.timeout - count);
      gtk_progress_bar_set_text (GTK_PROGRESS_BAR (w), lbl);
      g_free (lbl);
    }
  count++;

  return TRUE;
}

GtkWidget *
create_dialog ()
{
  GtkWidget *dlg;
  GtkWidget *hbox, *vbox, *hbox2;
  GtkWidget *image;
  GtkWidget *text;
  GtkWidget *main_widget = NULL;
  GtkWidget *topb = NULL;
  GError *err = NULL;

  /* create dialog window */
  dlg = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dlg), options.data.dialog_title);
#if !GTK_CHECK_VERSION (2,91,0)
  gtk_dialog_set_has_separator (GTK_DIALOG (dlg), options.data.dialog_sep);
#endif

  /* set window icon */
  if (options.data.window_icon)
    {
      if (g_file_test (options.data.window_icon, G_FILE_TEST_EXISTS))
        {
          gtk_window_set_icon_from_file (GTK_WINDOW (dlg),
                                         options.data.window_icon, &err);
          if (err)
            {
              g_printerr (_("Error loading window icon %s: %s\n"),
                          options.data.window_icon, err->message);
              g_error_free (err);
            }
        }
      else
        gtk_window_set_icon_name (GTK_WINDOW (dlg), options.data.window_icon);
    }

  /* set window behavior */
  if (options.data.sticky)
    gtk_window_stick (GTK_WINDOW (dlg));
  gtk_window_set_resizable (GTK_WINDOW (dlg), !options.data.fixed);
  gtk_window_set_keep_above (GTK_WINDOW (dlg), options.data.ontop);
  gtk_window_set_decorated (GTK_WINDOW (dlg), !options.data.undecorated);

  /* set window size and position */
  if (!options.data.geometry)
    {
      gtk_window_set_default_size (GTK_WINDOW (dlg),
                                   options.data.width, options.data.height);
      if (options.data.center)
        gtk_window_set_position (GTK_WINDOW (dlg), GTK_WIN_POS_CENTER);
      else if (options.data.mouse)
        gtk_window_set_position (GTK_WINDOW (dlg), GTK_WIN_POS_MOUSE);
    }

  /* create timeout indicator widget */
  if (options.data.timeout)
    {
      if (G_LIKELY (options.data.to_indicator) &&
          g_ascii_strcasecmp (options.data.to_indicator, "none"))
        {
          topb = gtk_progress_bar_new ();
          gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (topb), 1.0);
          gtk_widget_set_name (topb, "yad-timeout-indicator");
        }
    }

  /* add top label widgets */
  hbox = hbox2 = gtk_hbox_new (FALSE, 0);
#if GTK_CHECK_VERSION (2, 14, 0)
  gtk_container_add (GTK_CONTAINER (gtk_dialog_get_content_area
                                    (GTK_DIALOG (dlg))), hbox);
#else
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dlg)->vbox), hbox);
#endif
  vbox = gtk_vbox_new (FALSE, 0);

  /* add timeout indicator */
  if (topb)
    {
      if (g_ascii_strcasecmp (options.data.to_indicator, "top") == 0)
	gtk_box_pack_start (GTK_BOX (vbox), topb, FALSE, FALSE, 2);
      else if (g_ascii_strcasecmp (options.data.to_indicator, "bottom") == 0)
	gtk_box_pack_end (GTK_BOX (vbox), topb, FALSE, FALSE, 2);
      else if (g_ascii_strcasecmp (options.data.to_indicator, "left") == 0)
	gtk_box_pack_start (GTK_BOX (hbox), topb, FALSE, FALSE, 2);
      else if (g_ascii_strcasecmp (options.data.to_indicator, "right") == 0)
	gtk_box_pack_end (GTK_BOX (hbox), topb, FALSE, FALSE, 2);
      if (settings.show_remain)
        {
          gchar *lbl = g_strdup_printf (_("%d sec"), options.data.timeout);
          gtk_progress_bar_set_text (GTK_PROGRESS_BAR (topb), lbl);
          g_free (lbl);
        }
    }

  /* must be after indicator! */
  gtk_box_pack_end (GTK_BOX (hbox), vbox, TRUE, TRUE, 0);

  if (options.data.image_on_top)
    {
      hbox2 = gtk_hbox_new (FALSE, 0);
      gtk_box_pack_start (GTK_BOX (vbox), hbox2, FALSE, FALSE, 0);
    }

  if (options.data.dialog_image)
    {
      GdkPixbuf *pb = NULL;

      pb = get_pixbuf (options.data.dialog_image, YAD_BIG_ICON);
      image = gtk_image_new_from_pixbuf (pb);
      g_object_unref (pb);

      gtk_misc_set_alignment (GTK_MISC (image), 0.5, 0.0);
      gtk_misc_set_padding (GTK_MISC (image), 5, 5);
      gtk_box_pack_start (GTK_BOX (hbox2), image, FALSE, FALSE, 2);
    }
  if (options.data.dialog_text)
    {
      gchar *buf = g_strcompress (options.data.dialog_text);
      text = gtk_label_new (NULL);
      if (options.data.no_markup)
        gtk_label_set_text (GTK_LABEL (text), buf);
      else
        gtk_label_set_markup (GTK_LABEL (text), buf);
      gtk_misc_set_alignment (GTK_MISC (text), 0.0, 0.5);
      if (options.data.image_on_top)
        gtk_box_pack_start (GTK_BOX (hbox2), text, FALSE, FALSE, 2);
      else
        gtk_box_pack_start (GTK_BOX (vbox), text, FALSE, FALSE, 2);
      g_free (buf);
    }

  /* add main widget */
  switch (options.mode)
    {
    case YAD_MODE_CALENDAR:
      main_widget = calendar_create_widget (dlg);
      break;
    case YAD_MODE_COLOR:
      main_widget = color_create_widget (dlg);
      break;
    case YAD_MODE_DND:
      dnd_init (dlg);
      break;
    case YAD_MODE_ENTRY:
      main_widget = entry_create_widget (dlg);
      break;
    case YAD_MODE_FILE:
      main_widget = file_create_widget (dlg);
      break;
    case YAD_MODE_FORM:
      main_widget = form_create_widget (dlg);
      break;
    case YAD_MODE_ICONS:
      main_widget = icons_create_widget (dlg);
      break;
    case YAD_MODE_LIST:
      main_widget = list_create_widget (dlg);
      break;
    case YAD_MODE_PROGRESS:
      main_widget = progress_create_widget (dlg);
      break;
    case YAD_MODE_SCALE:
      main_widget = scale_create_widget (dlg);
      break;
    case YAD_MODE_TEXTINFO:
      main_widget = text_create_widget (dlg);
      break;
    }
  if (main_widget)
    gtk_box_pack_start (GTK_BOX (vbox), main_widget, TRUE, TRUE, 2);

  /* add buttons */
  if (!options.data.no_buttons)
    {
      if (options.data.buttons)
        {
          GSList *tmp = options.data.buttons;

          do
            {
              YadButton *b = (YadButton *) tmp->data;
              gtk_dialog_add_button (GTK_DIALOG (dlg), b->name, b->response);
              tmp = tmp->next;
            }
          while (tmp != NULL);
        }
      else
        {
#if GTK_CHECK_VERSION (2, 14, 0)
          if (gtk_alternative_dialog_button_order (NULL))
            gtk_dialog_add_buttons (GTK_DIALOG (dlg),
                                    GTK_STOCK_OK, YAD_RESPONSE_OK,
                                    GTK_STOCK_CANCEL, YAD_RESPONSE_CANCEL,
                                    NULL);
          else
#endif
            gtk_dialog_add_buttons (GTK_DIALOG (dlg),
                                    GTK_STOCK_CANCEL, YAD_RESPONSE_CANCEL,
                                    GTK_STOCK_OK, YAD_RESPONSE_OK, NULL);
          gtk_dialog_set_default_response (GTK_DIALOG (dlg), YAD_RESPONSE_OK);
        }
    }
  else
    gtk_widget_hide (gtk_dialog_get_action_area (GTK_DIALOG (dlg)));

  gtk_widget_show_all (hbox);

  /* parse geometry, if given. must be after showing widget */
  if (options.data.geometry)
    gtk_window_parse_geometry (GTK_WINDOW (dlg), options.data.geometry);
 
  gtk_widget_show_all (dlg);

  /* set timeout */
  if (options.data.timeout)
    {
      g_timeout_add_seconds (options.data.timeout, timeout_cb, dlg);
      g_timeout_add_seconds (1, timeout_indicator_cb, topb);
    }

  return dlg;
}

void
print_result (void)
{
  switch (options.mode)
    {
    case YAD_MODE_CALENDAR:
      calendar_print_result ();
      break;
    case YAD_MODE_COLOR:
      color_print_result ();
      break;
    case YAD_MODE_ENTRY:
      entry_print_result ();
      break;
    case YAD_MODE_FILE:
      file_print_result ();
      break;
    case YAD_MODE_FORM:
      form_print_result ();
      break;
    case YAD_MODE_LIST:
      list_print_result ();
      break;
    case YAD_MODE_SCALE:
      scale_print_result ();
      break;
    case YAD_MODE_TEXTINFO:
      text_print_result ();
      break;
    }
}

gint
main (gint argc, gchar ** argv)
{
  GOptionContext *ctx;
  GError *err = NULL;
  gint w, h;
  gint ret = 0;
  struct sigaction sa;
  
  setlocale (LC_ALL, "");

#ifdef ENABLE_NLS
  bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);
#endif

  read_settings ();

  gtk_init (&argc, &argv);

  yad_options_init ();

  /* set default icons and icon theme */
  settings.icon_theme = gtk_icon_theme_get_default ();
  gtk_icon_size_lookup (GTK_ICON_SIZE_DIALOG, &w, &h);
  settings.big_fallback_image =
    gtk_icon_theme_load_icon (settings.icon_theme, "unknown", MIN (w, h),
                              GTK_ICON_LOOKUP_GENERIC_FALLBACK, NULL);
  gtk_icon_size_lookup (GTK_ICON_SIZE_MENU, &w, &h);
  settings.small_fallback_image =
    gtk_icon_theme_load_icon (settings.icon_theme, "unknown", MIN (w, h),
                              GTK_ICON_LOOKUP_GENERIC_FALLBACK, NULL);

  ctx = yad_create_context ();
  g_option_context_parse (ctx, &argc, &argv, &err);
  if (err)
    {
      g_printerr (_("Unable parse command line: %s\n"), err->message);
      return -1;
    }
  yad_set_mode ();

  /* set signal handlers */
  bzero (&sa, sizeof (struct sigaction));
  sa.sa_handler = sa_usr1;
  sigaction (SIGUSR1, &sa, NULL);
  sa.sa_handler = sa_usr2;
  sigaction (SIGUSR2, &sa, NULL);

  switch (options.mode)
    {
    case YAD_MODE_ABOUT:
      ret = yad_about ();
      break;
    case YAD_MODE_VERSION:
      g_print ("%s\n", VERSION);
      break;
    case YAD_MODE_NOTIFICATION:
      ret = yad_notification_run ();
      break;
    default:
      dialog = create_dialog ();
      if (options.mode == YAD_MODE_FILE)
        {
          /* show custom confirmation dialog */
          g_signal_connect (G_OBJECT (dialog), "response",
                            G_CALLBACK (confirm_overwrite_cb), NULL);
        }
      ret = gtk_dialog_run (GTK_DIALOG (dialog));
      if (options.data.always_print)
        print_result ();
      else if (ret == YAD_RESPONSE_OK && options.data.buttons == NULL)
        print_result ();
      else if (options.data.buttons && !(ret & 1))
        print_result ();
    }

  return ret;
}
