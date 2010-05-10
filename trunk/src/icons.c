
#include "yad.h"

static GtkWidget *icon_view;

static GtkTreeModel *
create_model ()
{
}

GtkWidget *
icons_create_widget (GtkWidget *dlg)
{
  GtkWidget *w;
  GtkTreeModel *model;

  w = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (w),
				  GTK_POLICY_AUTOMATIC,
				  GTK_POLICY_AUTOMATIC);

  model = create_model ();

  icon_view = gtk_icon_view_new_with_model (model);
  g_object_unref (model);

  gtk_container_add (GTK_CONTAINER (w), icon_view);

  return w;
}
