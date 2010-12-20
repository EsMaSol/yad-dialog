desktopdir = $(datadir)/applications
desktop_in_files = yad-icon-browser.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

man_MANS = yad.1

imagesdir = $(datadir)/pixmaps
images_DATA = yad.png

EXTRA_DIST = $(man_MANS) $(images_DATA) yad.spec.in $(desktop_in_files)

DISTCLEAN_FILES = $(desktop_DATA)
