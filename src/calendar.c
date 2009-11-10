
#include <config.h>

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
  gchar *format = options.calendar_data.date_format;
  gchar time_string[128];
  GDate *date = NULL;

  if (format == NULL)
    format = "%x";

  gtk_calendar_get_date (GTK_CALENDAR (calendar), &day, &month, &year);
  date = g_date_new_dmy (year, month + 1, day);
  g_date_strftime (time_string, 127, format, date);
  g_print ("%s\n", time_string);
}
