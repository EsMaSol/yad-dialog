#! /bin/sh

set -e

libtoolize --automake -f
aclocal
autoconf
autoheader
automake -a
