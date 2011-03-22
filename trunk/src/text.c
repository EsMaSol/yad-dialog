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

#include <errno.h>

#include <gdk/gdkkeysyms.h>
#include <pango/pango.h>

#include "yad.h"

static GtkWidget *text_view;
static GtkTextBuffer *text_buffer;
static GtkTextTag *uri_tag;
static GRegex *regex;

static gboolean
key_press_cb (GtkWidget *w, GdkEventKey *key, gpointer data)
{
#if GTK_CHECK_VERSION (2,91,0)
  if ((key->keyval == GDK_KEY_Return || key->keyval == GDK_KEY_KP_Enter) &&
      (key->state & GDK_CONTROL_MASK))
#else
    if ((key->keyval == GDK_Return || key->keyval == GDK_KP_Enter) && 
	(key->state & GDK_CONTROL_MASK))
#endif
      gtk_dialog_response (GTK_DIALOG (data), YAD_RESPONSE_OK);

  return FALSE;
}

static gboolean
uri_tag_event_cb (GtkTextTag *tag, GObject *object, GdkEvent *event,
		  GtkTextIter *iter, gpointer user_data)
{
  GtkTextIter start = *iter;
  GtkTextIter end = *iter;
  gchar *url, *cmdline;
        
  if (event->type == GDK_BUTTON_PRESS) 
    {
      gtk_text_iter_backward_to_tag_toggle (&start, tag);
      gtk_text_iter_forward_to_tag_toggle (&end, tag);
                
      url = gtk_text_iter_get_text (&start, &end);
      cmdline = g_strdup_printf ("xdg-open '%s'", url);
      g_free (url);

      g_spawn_command_line_async (cmdline, NULL);

      g_free (cmdline);
    }
                
  return TRUE;
}

static gboolean
motion_cb (GtkWidget *w, GdkEventMotion *ev, gpointer d)
{
  GSList *tag_list_p;
  GtkTextIter iter;
  GtkTextTag *tag = NULL;
  gint x, y;

  if (!GTK_IS_TEXT_VIEW (w))
    return FALSE;

  gdk_window_get_pointer (ev->window, &x, &y, NULL);
  gtk_text_view_get_iter_at_location (GTK_TEXT_VIEW (w), &iter, x, y);
  tag_list_p = tag_list = gtk_text_iter_get_tags (&iter);

  while (tag_list_p != NULL) 
    {
      if (g_object_get_data (G_OBJECT (tag_list->data), "is_link") != NULL) 
	{
	  tag = GTK_TEXT_TAG (tag_list_p->data);
	  break;
	}
      tag_list_p = tag_list_p->next;
    }
  g_slist_free (tag_list);

  if (tag != NULL) 
    {
      GdkCursor *cursor = gdk_cursor_new( GDK_HAND2);
      gdk_window_set_cursor (ev->window, cursor);
      gdk_cursor_unref (cursor);
    } 
  else
    {
      GdkCursor *cursor = gdk_cursor_new (GDK_XTERM);
      gdk_window_set_cursor (ev->window, cursor);
      gdk_cursor_unref (cursor);
    }
        
  return FALSE;
}

static void
linkify_buffer ()
{
  gchar *text;
  GtkTextIter start, end;
  GMatchInfo *match;
  gint i, err;

  gtk_text_buffer_get_bounds (text_buffer, &start, &end);
  text = gtk_text_buffer_get_text (text_buffer, &start, &end, FALSE);

  gtk_text_buffer_remove_all_tags (text_buffer, &start, &end);

  err = g_regex_match_all (regex, text, G_REGEX_MATCH_NOTEMPTY, &match);

  for (i = 1;  i < g_match_info_get_match_count (match); i++)
    {
      gint sp, ep;

      g_match_info_fetch_pos (match, i, &sp, &ep);

      gtk_text_buffer_get_iter_at_offset (text_buffer, &start, sp);
      gtk_text_buffer_get_iter_at_offset (text_buffer, &end, ep);

      gtk_text_buffer_apply_tag (text_buffer, uri_tag, &start, &end);
    }

  g_match_info_free (match);
  g_free(text);
}

static void
insert_text_cb (GtkTextBuffer *tb, GtkTextIter *loc, gchar *text, gint len, gpointer d)
{
  linkify_buffer ();
}

static void
delete_text_cb (GtkTextBuffer *tb, GtkTextIter *start, GtkTextIter *end, gpointer d)
{
  linkify_buffer ();
}

static gboolean
handle_stdin (GIOChannel * channel,
	      GIOCondition condition, gpointer data)
{
  gchar buf[1024];
  gsize len;

  if ((condition & G_IO_IN) || (condition & (G_IO_IN | G_IO_HUP)))
    {
      GError *err = NULL;
      gint status;

      while (channel->is_readable != TRUE) ;

      do
        {
          status = g_io_channel_read_chars (channel, buf, 1024, &len, &err);
          while (gtk_events_pending ())
            gtk_main_iteration ();
        }
      while (status == G_IO_STATUS_AGAIN);

      if (status != G_IO_STATUS_NORMAL)
        {
          if (err)
            {
              g_printerr ("yad_text_handle_stdin(): %s", err->message);
              g_error_free (err);
	      err = NULL;
            }
          return FALSE;
        }

      if (len > 0)
        {
	  GtkTextIter end;
          gchar *utftext;
          gsize localelen;
          gsize utflen;

          gtk_text_buffer_get_end_iter (text_buffer, &end);

          if (!g_utf8_validate (buf, len, NULL))
            {
              utftext =
                g_convert_with_fallback (buf, len, "UTF-8", "ISO-8859-1",
                                         NULL, &localelen, &utflen, NULL);
              gtk_text_buffer_insert (text_buffer, &end, utftext, utflen);
              g_free (utftext);
            }
          else
	    gtk_text_buffer_insert (text_buffer, &end, buf, len);

	  if (options.text_data.tail)
	    {
	      while (gtk_events_pending ())
		gtk_main_iteration ();
	      gtk_text_buffer_get_end_iter (text_buffer, &end);
	      gtk_text_view_scroll_to_iter (GTK_TEXT_VIEW (text_view), &end, 0, FALSE, 0, 0);
	    }
        }
    }
            
  return TRUE;
}

static void
fill_buffer_from_file ()
{
  GtkTextIter iter, end;
  FILE *f;
  gchar buf[2048];
  gint remaining = 0;

  if (options.common_data.uri == NULL)
    return;

  f = fopen (options.common_data.uri, "r");

  if (f == NULL)
    {
      g_printerr (_("Cannot open file '%s': %s\n"),
		  options.common_data.uri, g_strerror (errno));
      return;
    }

  gtk_text_buffer_get_iter_at_offset (text_buffer, &iter, 0);

  while (!feof (f))
    {
      gint count;
      const char *leftover;
      int to_read = 2047 - remaining;

      count = fread (buf + remaining, 1, to_read, f);
      buf[count + remaining] = '\0';

      g_utf8_validate (buf, count + remaining, &leftover);

      g_assert (g_utf8_validate (buf, leftover - buf, NULL));
      gtk_text_buffer_insert (text_buffer, &iter, buf, leftover - buf);

      remaining = (buf + remaining + count) - leftover;
      g_memmove (buf, leftover, remaining);

      if (remaining > 6 || count < to_read)
        break;
    }

  if (remaining)
    {
      g_printerr (_("Invalid UTF-8 data encountered reading file %s\n"),
		  options.common_data.uri);
      return;
    }

  /* We had a newline in the buffer to begin with. (The buffer always contains
   * a newline, so we delete to the end of the buffer to clean up.
   */

  gtk_text_buffer_get_end_iter (text_buffer, &end);
  gtk_text_buffer_delete (text_buffer, &iter, &end);
  gtk_text_buffer_set_modified (text_buffer, FALSE);

  if (options.text_data.tail)
    {
      while (gtk_events_pending ())
	gtk_main_iteration ();
      gtk_text_buffer_get_end_iter (text_buffer, &end);
      gtk_text_view_scroll_to_iter (GTK_TEXT_VIEW (text_view), &end, 0, FALSE, 0, 0);
    }
}

static void
fill_buffer_from_stdin ()
{
  GIOChannel *channel;

  channel = g_io_channel_unix_new (0);
  g_io_channel_set_encoding (channel, NULL, NULL);
  g_io_channel_set_flags (channel, G_IO_FLAG_NONBLOCK, NULL);
  g_io_add_watch (channel, G_IO_IN | G_IO_HUP, handle_stdin, NULL);
}

GtkWidget *
text_create_widget (GtkWidget * dlg)
{
  GtkWidget *w;

  w = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (w),
				  GTK_POLICY_AUTOMATIC,
				  GTK_POLICY_AUTOMATIC);

  text_view = gtk_text_view_new ();
  text_buffer = gtk_text_buffer_new (NULL);
  gtk_text_view_set_buffer (GTK_TEXT_VIEW (text_view), text_buffer);
  gtk_text_view_set_justification (GTK_TEXT_VIEW (text_view), options.text_data.justify);
  gtk_text_view_set_left_margin (GTK_TEXT_VIEW (text_view), options.text_data.margins);
  gtk_text_view_set_right_margin (GTK_TEXT_VIEW (text_view), options.text_data.margins);
  gtk_text_view_set_editable (GTK_TEXT_VIEW (text_view), options.common_data.editable);
  if (!options.common_data.editable)
    gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (text_view), FALSE);

  if (options.text_data.wrap)
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (text_view), GTK_WRAP_WORD_CHAR);

  if (options.text_data.fore)
    {
      GdkColor clr;

      if (gdk_color_parse (options.text_data.fore, &clr))
	gtk_widget_modify_text (text_view, GTK_STATE_NORMAL, &clr);
    }

  if (options.text_data.back)
    {
      GdkColor clr;

      if (gdk_color_parse (options.text_data.back, &clr))
	gtk_widget_modify_base (text_view, GTK_STATE_NORMAL, &clr);
    }

  if (options.common_data.font)
    {
      PangoFontDescription *fd =
	pango_font_description_from_string (options.common_data.font);
      gtk_widget_modify_font (text_view, fd);
      pango_font_description_free (fd);
    }

  /* Add submit on ctrl+enter */
  g_signal_connect (text_view, "key-press-event",
		    G_CALLBACK (key_press_cb), dlg);

  if (options.text_data.uri)
    {
      /* Create text tag for URI */
      uri_tag = gtk_text_buffer_create_tag (text_buffer, NULL,
					    "foreground", "blue",
					    "underline", PANGO_UNDERLINE_SINGLE,
					    NULL);
      g_object_set_data (G_OBJECT(uri_tag), "is_link", GINT_TO_POINTER (TRUE));
      g_signal_connect (G_OBJECT (uri_tag), "event", G_CALLBACK (uri_tag_event_cb), NULL);
      
      regex = g_regex_new (YAD_URL_REGEX, 
			   G_REGEX_CASELESS | G_REGEX_OPTIMIZE | G_REGEX_EXTENDED,
			   G_REGEX_MATCH_NOTEMPTY,
			   NULL);
	
      g_signal_connect_after (G_OBJECT (text_buffer), "insert-text", 
			      G_CALLBACK (insert_text_cb), NULL);
      
      if (options.common_data.editable)
	{
	  g_signal_connect_after (G_OBJECT (text_buffer), "delete-range",
				  G_CALLBACK (delete_text_cb), NULL);
	}
    }

  gtk_container_add (GTK_CONTAINER (w), text_view);

  if (options.common_data.uri)
    fill_buffer_from_file ();
  else
    fill_buffer_from_stdin ();

  return w;
}

void
text_print_result (void)
{
  GtkTextIter start, end;
  gchar *text;

  if (!options.common_data.editable)
    return;

  gtk_text_buffer_get_bounds (text_buffer, &start, &end);
  text = gtk_text_buffer_get_text (text_buffer, &start, &end, 0);
  g_print ("%s", text);
  g_free (text);
}
