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

#include "yad.h"

gint
yad_about (void)
{
  GtkWidget *dialog;
  const gchar *const authors[] = {
    "Victor Ananjevsky <ananasik@gmail.com>",
    NULL
  };
  gchar *translators = _("translator-credits");
  gchar *license = 
    _("This program is free software; you can redistribute it and/or modify "
      "it under the terms of the GNU General Public License as published by "
      "the Free Software Foundation; either version 3 of the License, or "
      "(at your option) any later version.\n\n"
      "This program is distributed in the hope that it will be useful, "
      "but WITHOUT ANY WARRANTY; without even the implied warranty of "
      "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the "
      "GNU General Public License for more details.\n\n"
      "You should have received a copy of the GNU General Public License "
      "along with this program; if not, write to the Free Software "
      "Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.");
  
  dialog = gtk_about_dialog_new ();
  
  g_object_set (G_OBJECT (dialog),
		"name", PACKAGE_NAME,
		"version", PACKAGE_VERSION,
		"copyright", "Copyright \xc2\xa9 2008-2010 Victor Ananjevsky <ananasik@gmail.com>",
		"comments", _("Yet Another Dialog\n(show dialog boxes from shell scripts)\n\nBased on Zenity code\n"),
		"authors", authors,
		"website", PACKAGE_URL,
		"translator-credits", translators,
		"wrap-license", TRUE, "license", license, 
		NULL);
  gtk_about_dialog_set_logo_icon_name (GTK_ABOUT_DIALOG (dialog), "yad");
  gtk_window_set_icon_name (GTK_WINDOW (dialog), "yad");

  return gtk_dialog_run (GTK_DIALOG (dialog));
}
