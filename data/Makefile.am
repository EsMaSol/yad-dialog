desktop_in_files = yad-icon-browser.desktop.in
if BUILD_IB
desktopdir = $(datadir)/applications
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@
endif

man_MANS = yad.1

imagesdir = $(datadir)/pixmaps
images_DATA = yad.png

m4dir = $(datadir)/aclocal
m4_DATA = yad.m4

EXTRA_DIST = $(man_MANS) $(images_DATA) yad.spec.in yad.m4.in $(desktop_in_files)

DISTCLEAN_FILES = $(desktop_DATA) $(m4_DATA) yad.spec
