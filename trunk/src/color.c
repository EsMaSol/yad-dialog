
#include <config.h>

#include "yad.h"

static GtkWidget *color;

GtkWidget *
color_create_widget (GtkWidget *dlg)
{
  GtkWidget *w;

  color = w = gtk_color_selection_new ();

  if (options.color_data.init_color)
    {
      GdkColor c;
      
      if (gdk_color_parse (options.color_data.init_color, &c))
	gtk_color_selection_set_current_color (GTK_COLOR_SELECTION (w), &c);
    }

  return w;
}

void
color_print_result (void)
{
  GdkColor c;
  gchar *cs;

  gtk_color_selection_get_current_color (GTK_COLOR_SELECTION (color), &c);
  cs = gdk_color_to_string (&c);
  if (options.color_data.extra)
    g_print ("%s\n", cs);
  else
    g_printf ("#%c%c%c%c%c%c\n", cs[1], cs[2], cs[5], cs[6], cs[9], cs[10]);
  g_free (cs);
}
