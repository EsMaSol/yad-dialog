
#ifndef _YAD_H_
#define _YAD_H_

#include <config.h>

#include <gtk/gtk.h>
#include <glib/gi18n.h>

G_BEGIN_DECLS

#define YAD_RESPONSE_OK         0
#define YAD_RESPONSE_CANCEL     1
#define YAD_RESPONSE_TIMEOUT   	70
#define YAD_RESPONSE_ESC   	252

typedef enum {
  YAD_MODE_MESSAGE,
  YAD_MODE_CALENDAR,
  YAD_MODE_COLOR,
  YAD_MODE_DND,
  YAD_MODE_ENTRY,
  YAD_MODE_FILE,
  YAD_MODE_FORM,
  YAD_MODE_ICONS,
  YAD_MODE_LIST,
  YAD_MODE_NOTIFICATION,
  YAD_MODE_PROGRESS,
  YAD_MODE_SCALE,
  YAD_MODE_TEXTINFO,
  YAD_MODE_ABOUT,
  YAD_MODE_VERSION,
} YadDialogMode;

typedef enum {
  YAD_FIELD_SIMPLE = 0,
  YAD_FIELD_HIDDEN,
  YAD_FIELD_READ_ONLY,
  YAD_FIELD_NUM,
  YAD_FIELD_CHECK,
  YAD_FIELD_COMBO,
} YadFieldType;

typedef enum {
  YAD_COLUMN_TEXT = 0,
  YAD_COLUMN_NUM,
  YAD_COLUMN_CHECK,
  YAD_COLUMN_IMAGE,
  YAD_COLUMN_TOOLTIP,
} YadColumnType;

typedef enum {
  YAD_BIG_ICON = 0,
  YAD_SMALL_ICON,
} YadIconSize;

typedef struct {
  gchar *name;
  gint response;
} YadButton;

typedef struct {
  gchar *name;
  YadFieldType type;
} YadField;

typedef struct {
  gchar *name;
  YadColumnType type;
} YadColumn;

typedef struct {
  gchar *dialog_title;
  gchar *window_icon;
  gint width;
  gint height;
  gchar *geometry;
  guint timeout;
  gchar *to_indicator;
  gchar *dialog_text;
  gchar *dialog_image;
  gboolean image_on_top;
#if !GTK_CHECK_VERSION (2,91,0)
  gboolean dialog_sep;
#endif
  GSList *buttons;
  gboolean no_buttons;
  gboolean no_markup;
  /* window settings */
  gboolean sticky;
  gboolean fixed;
  gboolean ontop;
  gboolean center;
  gboolean mouse;
  gboolean undecorated;
} YadData;

typedef struct {
  gint day;
  gint month;
  gint year;
  gchar *date_format;
} YadCalendarData;

typedef struct {
  gchar *init_color;
  gboolean use_palette;
  gchar *palette;
  gboolean extra;
} YadColorData;

typedef struct {
  gchar *entry_text;
  gchar *entry_label;
  gboolean hide_text;
  gboolean completion;
} YadEntryData;

typedef struct {
  gboolean directory;
  gboolean save;
  gboolean confirm_overwrite;
  gchar *confirm_text;
  gchar **filter;
} YadFileData;

typedef struct {
  GSList *fields;
} YadFormData;

typedef struct {
  gchar *directory;
  gboolean compact;
  gboolean generic;
  gboolean stdin;
  guint width;
  gchar *term;
} YadIconsData;

typedef struct {
  GSList *columns;
  gboolean no_headers;
  gboolean checkbox;
  gboolean print_all;
  gint print_column;
  gint hide_column;
} YadListData;

typedef struct {
  gboolean listen;
} YadNotificationData;

typedef struct {
  gchar *progress_text;
  gboolean pulsate;
  gboolean autoclose;
  gboolean autokill;
  gdouble percentage;
  gboolean rtl;
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
  gchar *fore;
  gchar *back;
  gchar *font;
  gboolean wrap;
  GtkJustification justify;
  gboolean tail;
} YadTextData;

typedef struct {
  gchar *uri;
  gchar *separator;
  gboolean editable;
  gboolean multi;
  gchar *command;
} YadCommonData;

typedef struct {
  YadDialogMode mode;

  YadData data;
  YadCommonData common_data;

  YadCalendarData calendar_data;
  YadColorData color_data;
  YadEntryData entry_data;
  YadFileData file_data;
  YadFormData form_data;
  YadIconsData icons_data;
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
  gchar *to_indicator;
  gboolean show_remain;
  gboolean rules_hint;
  gboolean always_selected;
  gchar *menu_sep;
#if !GTK_CHECK_VERSION (2,91,0)
  gboolean dlg_sep;
#endif
  gboolean combo_always_editable;
  gboolean expand_palette;
  GtkIconTheme *icon_theme;
  GdkPixbuf *big_fallback_image;
  GdkPixbuf *small_fallback_image;
  gchar *term;
} YadSettings;

extern YadSettings settings;

void yad_options_init (void);
GOptionContext * yad_create_context (void);
void yad_set_mode (void);

GtkWidget * calendar_create_widget (GtkWidget *dlg);
GtkWidget * color_create_widget (GtkWidget *dlg);
GtkWidget * entry_create_widget (GtkWidget *dlg);
GtkWidget * file_create_widget (GtkWidget *dlg);
GtkWidget * form_create_widget (GtkWidget *dlg);
GtkWidget * icons_create_widget (GtkWidget *dlg);
GtkWidget * list_create_widget (GtkWidget *dlg);
GtkWidget * progress_create_widget (GtkWidget *dlg);
GtkWidget * scale_create_widget (GtkWidget *dlg);
GtkWidget * text_create_widget (GtkWidget *dlg);

void confirm_overwrite_cb (GtkDialog *dlg, gint id, gpointer data);

void calendar_print_result (void);
void color_print_result (void);
void entry_print_result (void);
void file_print_result (void);
void form_print_result (void);
void list_print_result (void);
void progress_print_result (void);
void scale_print_result (void);
void text_print_result (void);

void dnd_init (GtkWidget *w);

gint yad_notification_run (void);

gint yad_about (void);

void read_settings (void);

GdkPixbuf * get_pixbuf (gchar *name, YadIconSize size);

inline void strip_new_line (gchar *str);

G_END_DECLS

#endif /* _YAD_H_ */
