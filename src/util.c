
#include <string.h>

#include <gtk/gtk.h>

inline void 
strip_new_line (gchar *str)
{
  gint nl = strlen (str) - 1;

  if (str[nl] == '\n')
    str[nl] = '\0';                                      
}
