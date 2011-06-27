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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) 2008-2011, Victor Ananjevsky <ananasik@gmail.com>
 */

#include "yad.h"

static GtkWidget *scale;

static void
value_changed_cb (GtkWidget *w, gpointer data)
{
  g_print ("%.0f\n", gtk_range_get_value (GTK_RANGE (w)));
}

GtkWidget *
scale_create_widget (GtkWidget *dlg)
{
  GtkWidget *w;

  if (options.scale_data.min_value >= options.scale_data.max_value)
    {
      g_printerr (_("Maximum value must be greater than minimum value.\n"));
      return NULL;
    }

  /* check for initial value */
  if (options.scale_data.have_value)
    {
      if (options.scale_data.value < options.scale_data.min_value)
	{
	  g_printerr (_("Initial value less than minimal.\n"));
	  options.scale_data.value = options.scale_data.min_value;
	}
      else if (options.scale_data.value > options.scale_data.max_value)
	{
	  g_printerr (_("Initial value greater than maximum.\n"));
	  options.scale_data.value = options.scale_data.max_value;
	}
    }
  else
    options.scale_data.value = options.scale_data.min_value;

  if (options.scale_data.vertical)
    {
      w = scale = gtk_vscale_new_with_range (options.scale_data.min_value,
					     options.scale_data.max_value,
					     options.scale_data.step);
      gtk_range_set_inverted (GTK_RANGE (w), !options.scale_data.invert);
    }
  else
    {
      w = scale = gtk_hscale_new_with_range (options.scale_data.min_value,
					     options.scale_data.max_value,
					     options.scale_data.step);
      gtk_range_set_inverted (GTK_RANGE (w), options.scale_data.invert);
    }
  gtk_range_set_value (GTK_RANGE (w), options.scale_data.value);

  if (options.scale_data.print_partial)
    g_signal_connect (G_OBJECT (w), "value-changed",
		      G_CALLBACK (value_changed_cb), NULL);

  if (options.scale_data.hide_value)
    gtk_scale_set_draw_value (GTK_SCALE (w), FALSE);

  return w;
}

void
scale_print_result (void)
{
  g_print ("%.0f\n", gtk_range_get_value (GTK_RANGE (scale)));
}
