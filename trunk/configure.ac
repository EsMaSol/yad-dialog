AC_INIT([Yad],[0.0.7],[http://code.google.com/p/yad/issues/entry], yad)
AM_INIT_AUTOMAKE([1.11 foreign dist-bzip2 no-dist-gzip])
AM_SILENT_RULES([yes])

AC_PREREQ([2.59])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([src])

AC_CONFIG_MACRO_DIR([m4])

AM_MAINTAINER_MODE

IT_PROG_INTLTOOL([0.40.0])

AC_PROG_CC

GTK_REQUIRED=2.12.0

PKG_CHECK_MODULES([YAD],[gtk+-2.0 >= $GTK_REQUIRED glib-2.0])
AC_SUBST([YAD_CFLAGS])
AC_SUBST([YAD_LIBS])

GTK_DISABLE_DEPRECATED

# *******************************
# Internationalization
# ******************************* 

GETTEXT_PACKAGE=yad
AC_SUBST([GETTEXT_PACKAGE])
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],["$GETTEXT_PACKAGE"],[Gettext domain])

AM_GLIB_GNU_GETTEXT

# *******************************

AC_SUBST([AM_CPPFLAGS])
AC_SUBST([AM_CFLAGS])
AC_SUBST([AM_LDFLAGS])

# *******************************

AC_CONFIG_FILES([
Makefile 
po/Makefile.in
src/Makefile
data/Makefile
examples/Makefile
])

AC_OUTPUT
