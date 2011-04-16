AC_INIT([YAD], [0.10.2], [http://code.google.com/p/yad/issues/entry], yad, [http://code.google.com/p/yad])
AM_INIT_AUTOMAKE([1.11 foreign dist-xz no-dist-gzip])
AM_SILENT_RULES([yes])

AC_PREREQ([2.59])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([src])

AM_MAINTAINER_MODE

IT_PROG_INTLTOOL([0.40.0])

AC_PROG_CC

GTK2_REQUIRED=2.16.0
GTK3_REQUIRED=2.91.0

PKG_CHECK_MODULES([YAD], [gtk+-3.0 >= $GTK3_REQUIRED], [gtk3=yes], [gtk3=no])
if test x$gtk3 = xno
then
    PKG_CHECK_MODULES([YAD], [gtk+-2.0 >= $GTK2_REQUIRED])
fi
AC_SUBST([YAD_CFLAGS])
AC_SUBST([YAD_LIBS])

GTK_DISABLE_DEPRECATED

dnl icon browser
AC_ARG_ENABLE([icon-browser],
	[AS_HELP_STRING([--enable-icon-browser],
			[Build YAD icon browser])],
	[build_ib=$enableval], [build_ib=no])
AM_CONDITIONAL([BUILD_IB], [test x$build_ib = xyes])

dnl rgb.txt
AC_ARG_WITH([rgb],
	[AS_HELP_STRING([--with-rgb=PATH],
			[set path to rgb.txt file (default - /etc/X11/rgb.txt)])],
	[AC_DEFINE_UNQUOTED([RGB_FILE], ["$withval"], [Path to rgb.txt file])],
	[AC_DEFINE([RGB_FILE], ["/etc/X11/rgb.txt"], [Path to rgb.txt file])])

# *******************************
# Internationalization
# ******************************* 

GETTEXT_PACKAGE=yad
AC_SUBST([GETTEXT_PACKAGE])
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"], [Gettext domain])

AM_GLIB_GNU_GETTEXT

# *******************************

AC_SUBST([AM_CFLAGS])
AC_SUBST([AM_CPPFLAGS])
AC_SUBST([AM_LDFLAGS])

# *******************************

AC_CONFIG_FILES([
Makefile 
po/Makefile.in
src/Makefile
data/Makefile
data/yad.m4
data/yad.spec
])

AC_OUTPUT

echo
echo "Build icon browser - $build_ib"
echo
