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
 * Copyright (C) 2008-2011, Victor Ananjevsky <ananasik@gmail.com>
 */

#include <gtk/gtkprintunixdialog.h>

#include "yad.h"

static void
text_size_allocate_cb (GtkWidget *w, GtkAllocation *al, gpointer data)
{
  gtk_widget_set_size_request (w, al->width, -1);
}

gint
yad_print_run (void)
{
  GtkWidget *dlg;
  GtkWidget *box, *img, *lbl;
  gint ret = 0;

  /* create print dialog */
  dlg = gtk_print_unix_dialog_new (options.data.dialog_title, NULL);
  gtk_print_unix_dialog_set_embed_page_setup (GTK_PRINT_UNIX_DIALOG (dlg), TRUE);

  if (settings.print_settings)
    {
      gtk_print_unix_dialog_set_settings (GTK_PRINT_UNIX_DIALOG (dlg),
					  settings.print_settings);
    }
  if (settings.page_setup)
    {
      gtk_print_unix_dialog_set_page_setup (GTK_PRINT_UNIX_DIALOG (dlg),
					    settings.page_setup);
    }

  /* set window behavior */
  gtk_widget_set_name (dlg, "yad-dialog-window");
  if (options.data.sticky)
    gtk_window_stick (GTK_WINDOW (dlg));
  gtk_window_set_resizable (GTK_WINDOW (dlg), !options.data.fixed);
  gtk_window_set_keep_above (GTK_WINDOW (dlg), options.data.ontop);
  gtk_window_set_decorated (GTK_WINDOW (dlg), !options.data.undecorated);
  gtk_window_set_skip_taskbar_hint (GTK_WINDOW (dlg), options.data.skip_taskbar);
  gtk_window_set_skip_pager_hint (GTK_WINDOW (dlg), options.data.skip_taskbar);

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
  else
    {
      /* parse geometry, if given. must be after showing widget */
      gtk_widget_realize (dlg);
      gtk_window_parse_geometry (GTK_WINDOW (dlg), options.data.geometry);
    }
  
  /* create yad's top box */
  if (options.data.dialog_text || options.data.dialog_image)
    {
#if !GTK_CHECK_VERSION(3,0,0)
      box = gtk_hbox_new (FALSE, 0);
#else
      box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
#endif

      if (options.data.dialog_image)
	{
	  GdkPixbuf *pb = NULL;

	  pb = get_pixbuf (options.data.dialog_image, YAD_BIG_ICON);
	  img = gtk_image_new_from_pixbuf (pb);
	  if (pb)
	    g_object_unref (pb);

	  gtk_widget_set_name (img, "yad-dialog-image");
	  gtk_box_pack_start (GTK_BOX (box), img, FALSE, FALSE, 2);
	}
      if (options.data.dialog_text)
	{
	  gchar *buf = g_strcompress (options.data.dialog_text);

	  lbl = gtk_label_new (NULL);
	  if (!options.data.no_markup)
	    gtk_label_set_markup (GTK_LABEL (lbl), buf);
	  else
	    gtk_label_set_text (GTK_LABEL (lbl), buf);
	  gtk_widget_set_name (lbl, "yad-dialog-label");
	  gtk_label_set_selectable (GTK_LABEL (lbl), options.data.selectable_labels);
	  gtk_misc_set_alignment (GTK_MISC (lbl), options.data.text_align, 0.5);
	  if (options.data.geometry || options.data.width != -1)
	    gtk_label_set_line_wrap (GTK_LABEL (lbl), TRUE);
	  gtk_box_pack_start (GTK_BOX (box), lbl, TRUE, TRUE, 2);
	  g_signal_connect (G_OBJECT (lbl), "size-allocate",
			    G_CALLBACK (text_size_allocate_cb), NULL);     
	  g_free (buf);
	}

      /* add tob box to dialog */
      gtk_box_pack_start (GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG (dlg))),
			  box, TRUE, TRUE, 5);
      gtk_box_reorder_child (GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG (dlg))),
			     box, 0);
    }

  gtk_widget_show_all (dlg);
  ret = gtk_dialog_run (GTK_DIALOG (dlg));

  return ret;
}
