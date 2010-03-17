
#include <string.h>

#include "yad.h"

#define SETTINGS_FILE "yad.conf"

YadSettings settings;

static void
create_settings (gchar *filename)
{
  GKeyFile *kf;
  gchar *context;

  kf = g_key_file_new ();

  g_key_file_set_string (kf, "General", "separator", settings.sep);
  g_key_file_set_comment (kf, "General", "separator", "Default separator for output", NULL);
  g_key_file_set_string (kf, "General", "menu_separator", settings.menu_sep);
  g_key_file_set_comment (kf, "General", "menu_separator", 
			  "Default separator for notification icon menu (single char)", NULL);
  g_key_file_set_boolean (kf, "General", "dialog_separator", settings.dlg_sep);
  g_key_file_set_comment (kf, "General", "dialog_separator", "Enable separator between dialog and buttons", NULL);
  g_key_file_set_integer (kf, "General", "width", settings.width);
  g_key_file_set_comment (kf, "General", "width", "Default dialog width", NULL);
  g_key_file_set_integer (kf, "General", "height", settings.height);
  g_key_file_set_comment (kf, "General", "height", "Default dialog height", NULL);
  g_key_file_set_integer (kf, "General", "timeout", settings.timeout);
  g_key_file_set_comment (kf, "General", "timeout", "Default timeout (0 for no timeout)", NULL);
  g_key_file_set_boolean (kf, "General", "rules_hint", settings.rules_hint);
  g_key_file_set_comment (kf, "General", "rules_hint", "Enable rules hints in list widget", NULL);
  g_key_file_set_boolean (kf, "General", "combo_always_editable", settings.combo_always_editable);
  g_key_file_set_comment (kf, "General", "combo_always_editable", "Combo-box in entry dialog is always editable", NULL);
  g_key_file_set_boolean (kf, "General", "expand_palette", settings.expand_palette);
  g_key_file_set_comment (kf, "General", "expand_palette", "Expand list of predefined colors in color dialog", NULL);
  g_key_file_set_integer (kf, "General", "icon_size", settings.icon_size);
  g_key_file_set_comment (kf, "General", "icon_size", "Default icon size for lis widget", NULL);
  context = g_key_file_to_data (kf, NULL, NULL);

  g_key_file_free (kf);

  g_file_set_contents (filename, context, -1, NULL);
  g_free (context);
}

void
read_settings (void)
{
  GKeyFile *kf;
  gchar *filename;

  /* set defaults */
  settings.sep = "|";
  settings.width = settings.height = -1;
  settings.timeout = 0;
  settings.rules_hint = TRUE;
  settings.menu_sep = "!";
  settings.dlg_sep = FALSE;
  settings.combo_always_editable = FALSE;
  settings.expand_palette = FALSE;
  settings.icon_size = 16;

  filename = g_build_filename (g_get_user_config_dir (), 
			       SETTINGS_FILE, NULL);

  if (g_file_test (filename, G_FILE_TEST_EXISTS))
    {
      kf = g_key_file_new ();

      if (g_key_file_load_from_file (kf, filename, G_KEY_FILE_NONE, NULL))
	{
	  if (g_key_file_has_key (kf, "General", "separator", NULL))
	    settings.sep = g_key_file_get_string (kf, "General", "separator", NULL);
	  if (g_key_file_has_key (kf, "General", "menu_separator", NULL))
	    settings.menu_sep = g_key_file_get_string (kf, "General", "menu_separator", NULL);
	  if (g_key_file_has_key (kf, "General", "dialog_separator", NULL))
	    settings.dlg_sep = g_key_file_get_boolean (kf, "General", "dialog_separator", NULL);
	  if (g_key_file_has_key (kf, "General", "width", NULL))
	    settings.width = g_key_file_get_integer (kf, "General", "width", NULL);
	  if (g_key_file_has_key (kf, "General", "height", NULL))
	    settings.height = g_key_file_get_integer (kf, "General", "height", NULL);
	  if (g_key_file_has_key (kf, "General", "timeout", NULL))
	    settings.timeout = g_key_file_get_integer (kf, "General", "timeout", NULL);
	  if (g_key_file_has_key (kf, "General", "rules_hint", NULL))
	    settings.rules_hint = g_key_file_get_boolean (kf, "General", "rules_hint", NULL);
	  if (g_key_file_has_key (kf, "General", "combo_always_editable", NULL))
	    settings.combo_always_editable = g_key_file_get_boolean (kf, "General", "combo_always_editable", NULL);
	  if (g_key_file_has_key (kf, "General", "expand_palette", NULL))
	    settings.expand_palette = g_key_file_get_boolean (kf, "General", "expand_palette", NULL);
	  if (g_key_file_has_key (kf, "General", "icon_size", NULL))
	    settings.icon_size = g_key_file_get_integer (kf, "General", "icon_size", NULL);
	}

      g_key_file_free (kf);
    }
  else
    create_settings (filename);

  g_free (filename);
}

GdkPixbuf * 
get_pixbuf (gchar *name)
{
  GdkPixbuf *pb;
  GError *err = NULL;

  if (g_file_test (name, G_FILE_TEST_EXISTS))
    {
      pb = gdk_pixbuf_new_from_file (name, &err);
      if (!pb)
	{
	  g_warning ("yad_get_pixbuf(): %s", err->message);
	  g_error_free (err);
	}
    }
  else
    {
      pb = gtk_icon_theme_load_icon (settings.icon_theme, name, settings.icon_size, 
				     GTK_ICON_LOOKUP_GENERIC_FALLBACK, NULL);
    }

  if (!pb)
    {
      /* get fallback pixbuf */
      pb = gtk_icon_theme_load_icon (settings.icon_theme, "unknown", settings.icon_size, 
				     GTK_ICON_LOOKUP_GENERIC_FALLBACK, NULL);
    }

  return pb;
}

inline void 
strip_new_line (gchar *str)
{
  gint nl = strlen (str) - 1;

  if (str[nl] == '\n')
    str[nl] = '\0';                                      
}
