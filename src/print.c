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

#define HEADER_HEIGHT (10*72/25.4)
#define HEADER_GAP (3*72/25.4)

static GtkPrintSettings *print_settings = NULL;
static GtkPageSetup *page_setup = NULL;

static void
begin_print_text (GtkPrintOperation *op, GtkPrintContext *cnt, gpointer data)
{
}

static void
draw_page_text (GtkPrintOperation *op, GtkPrintContext *cnt, gint page, gpointer data)
{
}

static void
draw_page_image (GtkPrintOperation *op, GtkPrintContext *cnt, gint page, gpointer data)
{
  cairo_t *cr;
  GdkPixbuf *pb, *spb;
  guint iw, ih;
  gdouble pw, ph;
  gdouble factor;

  cr = gtk_print_context_get_cairo_context (cnt);
  pb = gdk_pixbuf_new_from_file (options.common_data.uri, NULL);

  /* scale image to page size */
  pw = gtk_page_setup_get_paper_width (page_setup, GTK_UNIT_POINTS)
    - gtk_page_setup_get_left_margin (page_setup, GTK_UNIT_POINTS)
    - gtk_page_setup_get_right_margin (page_setup, GTK_UNIT_POINTS);
  ph = gtk_page_setup_get_paper_height (page_setup, GTK_UNIT_POINTS)
    - gtk_page_setup_get_top_margin (page_setup, GTK_UNIT_POINTS)
    - gtk_page_setup_get_bottom_margin (page_setup, GTK_UNIT_POINTS);

  iw = gdk_pixbuf_get_width (pb);
  ih = gdk_pixbuf_get_height (pb);

  if (pw < iw || ph < ih)
    {
      factor = MIN (pw / iw, ph / ih);
      factor = (factor > 1.0) ? 1.0 : factor;    
      spb = gdk_pixbuf_scale_simple (pb, iw * factor, ih * factor, GDK_INTERP_HYPER);
    }
  else
    spb = g_object_ref (pb);
  g_object_unref (pb);

  /* add image to surface */
  gdk_cairo_set_source_pixbuf (cr, spb, (pw - iw) / 2, (ph - ih) / 2);
  cairo_paint (cr);
  g_object_unref (spb);
}

static void
begin_print_raw (GtkPrintOperation *op, GtkPrintContext *cnt, gpointer data)
{
}

static void
draw_page_raw (GtkPrintOperation *op, GtkPrintContext *cnt, gint page, gpointer data)
{
}

static void
size_allocate_cb (GtkWidget *w, GtkAllocation *al, gpointer data)
{
  gtk_widget_set_size_request (w, al->width, -1);
}

gint
yad_print_run (void)
{
  GtkWidget *dlg;
  GtkWidget *box, *img, *lbl;
  GtkPrintOperation *op;
  GtkPrintOperationAction act = GTK_PRINT_OPERATION_ACTION_PRINT;
  gint ret = 0;
  GError *err = NULL;

  /* check if file is exists */
  if (options.common_data.uri && options.common_data.uri[0])
    {
      if (!g_file_test (options.common_data.uri, G_FILE_TEST_EXISTS))
	{
	  g_printerr (_("File %s not found.\n"), options.common_data.uri);
	  return 1;
	}
    }
  else
    {
      g_printerr (_("Filename is not specified.\n"));
      return 1;
    }

  /* create print dialog */
  dlg = gtk_print_unix_dialog_new (options.data.dialog_title, NULL);
  gtk_print_unix_dialog_set_embed_page_setup (GTK_PRINT_UNIX_DIALOG (dlg), TRUE);
  gtk_print_unix_dialog_set_manual_capabilities (GTK_PRINT_UNIX_DIALOG (dlg),
						 GTK_PRINT_CAPABILITY_PAGE_SET |
						 GTK_PRINT_CAPABILITY_COPIES |
						 GTK_PRINT_CAPABILITY_COLLATE |
						 GTK_PRINT_CAPABILITY_REVERSE |
						 GTK_PRINT_CAPABILITY_PREVIEW |
						 GTK_PRINT_CAPABILITY_NUMBER_UP |
						 GTK_PRINT_CAPABILITY_NUMBER_UP_LAYOUT);

  if (print_settings)
    gtk_print_unix_dialog_set_settings (GTK_PRINT_UNIX_DIALOG (dlg), print_settings);
  if (page_setup)
    gtk_print_unix_dialog_set_page_setup (GTK_PRINT_UNIX_DIALOG (dlg), page_setup);

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
	  g_signal_connect (G_OBJECT (lbl), "size-allocate", G_CALLBACK (size_allocate_cb), NULL);     
	  g_free (buf);
	}

      /* add tob box to dialog */
      gtk_widget_show_all (box);
      gtk_box_pack_start (GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG (dlg))),
			  box, TRUE, TRUE, 5);
      gtk_box_reorder_child (GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG (dlg))),
			     box, 0);
    }

  gtk_widget_show (dlg);
  switch (gtk_dialog_run (GTK_DIALOG (dlg)))
    {
    case GTK_RESPONSE_APPLY:                  /* ask for preview */
      act = GTK_PRINT_OPERATION_ACTION_PREVIEW;
    case GTK_RESPONSE_OK:                     /* run print */
      op = gtk_print_operation_new ();
      gtk_print_operation_set_unit (op, GTK_UNIT_POINTS);
      print_settings = gtk_print_unix_dialog_get_settings (GTK_PRINT_UNIX_DIALOG (dlg));
      gtk_print_operation_set_print_settings (op, print_settings);
      page_setup = gtk_print_unix_dialog_get_page_setup (GTK_PRINT_UNIX_DIALOG (dlg));
      gtk_print_operation_set_default_page_setup (op, page_setup);
      
      switch (options.print_data.type)
	{
	case YAD_PRINT_TEXT:
	  g_signal_connect (G_OBJECT (op), "begin-print", G_CALLBACK (begin_print_text), NULL);
	  g_signal_connect (G_OBJECT (op), "draw-page", G_CALLBACK (draw_page_text), NULL);
	  break;
	case YAD_PRINT_IMAGE:
	  gtk_print_operation_set_n_pages (op, 1);
	  g_signal_connect (G_OBJECT (op), "draw-page", G_CALLBACK (draw_page_image), NULL);
	  break;
	case YAD_PRINT_RAW:
	  g_signal_connect (G_OBJECT (op), "begin-print", G_CALLBACK (begin_print_raw), NULL);
	  g_signal_connect (G_OBJECT (op), "draw-page", G_CALLBACK (draw_page_raw), NULL);
	  break;
	}

      gtk_print_operation_set_show_progress (op, TRUE);
      gtk_print_operation_run (op, act, NULL, &err);
      if (err)
	{
	  printf (_("Printing failed: %s\n"), err->message);
	  ret = 1;
	}
      break;
    default:
      ret = 1;
      break;
    }

  gtk_widget_destroy (dlg);
  return ret;
}
