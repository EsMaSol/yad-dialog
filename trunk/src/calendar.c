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

#include "yad.h"

static GtkWidget *calendar;

static void
double_click_cb (GtkWidget *w, gpointer data)
{
  gtk_dialog_response (GTK_DIALOG (data), YAD_RESPONSE_OK);
}

GtkWidget *
calendar_create_widget (GtkWidget *dlg)
{
  GtkWidget *w;

  w = calendar = gtk_calendar_new ();

  /* FIXME: add "show details" to widget */

  if (options.calendar_data.month > 0 || options.calendar_data.year > 0)
    gtk_calendar_select_month (GTK_CALENDAR (w),
			       options.calendar_data.month - 1,
			       options.calendar_data.year);
  if (options.calendar_data.day > 0)
    gtk_calendar_select_day (GTK_CALENDAR (w),
			     options.calendar_data.day);

  g_signal_connect (w, "day-selected-double-click",
  		    G_CALLBACK (double_click_cb), dlg);

  return w;
}

void
calendar_print_result (void)
{
  guint day, month, year;
  gchar *format = options.common_data.date_format;
  gchar time_string[128];
  GDate *date = NULL;

  if (format == NULL)
    format = "%x";

  gtk_calendar_get_date (GTK_CALENDAR (calendar), &day, &month, &year);
  date = g_date_new_dmy (year, month + 1, day);
  g_date_strftime (time_string, 127, format, date);
  g_print ("%s\n", time_string);
}
