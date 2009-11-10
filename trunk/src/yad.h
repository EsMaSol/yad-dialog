
#ifndef YAD_H
#define YAD_H

#include <gtk/gtk.h>
#include <glib/gi18n.h>

G_BEGIN_DECLS

#define YAD_RESPONSE_OK         0
#define YAD_RESPONSE_CANCEL     1
#define YAD_RESPONSE_TIMEOUT   	127
#define YAD_RESPONSE_ESC   	252

typedef enum
{
  MODE_MESSAGE,
  MODE_CALENDAR,
  MODE_COLOR,
  MODE_ENTRY,
  MODE_FILE,
  MODE_LIST,
  MODE_NOTIFICATION,
  MODE_PROGRESS,
  MODE_SCALE,
  MODE_TEXTINFO,
  MODE_ABOUT,
  MODE_VERSION,
} YadDialogMode;

typedef struct {
  gchar *name;
  gint response;
} YadButton;

typedef struct {
  gchar *dialog_title;
  gchar *window_icon;
  gint width;
  gint height;
  guint timeout;
  gchar *dialog_text;
  gchar *dialog_image;
  gboolean no_wrap;
  gboolean dialog_sep;
  GSList *buttons;
} YadData;

typedef struct {
  gint day;
  gint month;
  gint year;
  gchar *date_format;
} YadCalendarData;

typedef struct {
  gchar *init_color;
  gboolean extra;
} YadColorData;

typedef struct {
  gchar *entry_text;
  gboolean hide_text;
  gboolean completion;
} YadEntryData;

typedef struct {
  gboolean directory;
  gboolean save;
  gboolean confirm_overwrite;
  gchar **filter;
} YadFileData;

typedef struct {
  GSList *columns;
  gboolean checkbox;
  gboolean print_all;
  gint print_column;
  gint hide_column;
} YadListData;

typedef struct {
  gchar *command;
  gboolean listen;
} YadNotificationData;

typedef struct {
  gchar *progress_text;
  gboolean pulsate;
  gboolean autoclose;
  gboolean autokill;
  gdouble percentage;
} YadProgressData;

typedef struct {
  gint value;
  gint min_value;
  gint max_value;
  gint step;
  gboolean print_partial;
  gboolean hide_value;
} YadScaleData;

typedef struct {
  gchar *font;
} YadTextData;

typedef struct {
  gchar *uri;
  gchar *separator;
  gboolean editable;
  gboolean multi;
} YadCommonData;

typedef struct
{
  YadDialogMode mode;

  YadData data;
  YadCommonData common_data;

  YadCalendarData calendar_data;
  YadColorData color_data;
  YadEntryData entry_data;
  YadFileData file_data;
  YadListData list_data;
  YadNotificationData notification_data;
  YadProgressData progress_data;
  YadScaleData scale_data;
  YadTextData text_data;

  gchar **extra_data;
} YadOptions;

extern YadOptions options;

typedef struct {
  gchar *sep;
  guint width;
  guint height;
  guint timeout;
  gboolean rules_hint;
  gchar *menu_sep;
  gboolean dlg_sep;
} YadSettings;

extern YadSettings settings;

void yad_options_init (void);
GOptionContext * yad_create_context (void);
void yad_set_mode (void);

GtkWidget * calendar_create_widget (GtkWidget *dlg);
GtkWidget * color_create_widget (GtkWidget *dlg);
GtkWidget * entry_create_widget (GtkWidget *dlg);
GtkWidget * file_create_widget (GtkWidget *dlg);
GtkWidget * list_create_widget (GtkWidget *dlg);
GtkWidget * progress_create_widget (GtkWidget *dlg);
GtkWidget * scale_create_widget (GtkWidget *dlg);
GtkWidget * text_create_widget (GtkWidget *dlg);

void calendar_print_result (void);
void color_print_result (void);
void entry_print_result (void);
void file_print_result (void);
void list_print_result (void);
void progress_print_result (void);
void scale_print_result (void);
void text_print_result (void);

gint yad_notification_tun (void);

gint yad_about (void);

void read_settings (void);

inline void strip_new_line (gchar *str);

G_END_DECLS

#endif /* YAD_H */
