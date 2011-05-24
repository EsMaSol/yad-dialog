INCLUDES = 					\
	-DLOCALEDIR=\""$(datadir)/locale"\"	\
	@DISABLE_DEPRECATED@

bin_PROGRAMS = yad
bin_SCRIPTS = yad-zenity.sh

yad_SOURCES = 		\
	about.c		\
	calendar.c	\
	color.c		\
	dnd.c           \
	entry.c		\
	file.c	\
	font.c		\
	form.c		\
	icons.c		\
	list.c		\
	notification.c	\
	option.c	\
	progress.c	\
	scale.c		\
	text.c		\
	util.c		\
	main.c		\
	yad.h

yad_CFLAGS = $(YAD_CFLAGS)

yad_LDADD = $(YAD_LIBS)

if BUILD_IB
bin_PROGRAMS += yad-icon-browser

yad_icon_browser_SOURCES = browser.c
yad_icon_browser_CFLAGS = $(YAD_CFLAGS)
yad_icon_browser_LDADD = $(YAD_LIBS)
endif

DISTCLEANFILES =

EXTRA_DIST =


