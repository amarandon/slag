########################################################################
#
#   $Id: configure.ac,v 1.7 2007/01/26 07:39:55 rhizome Exp $
#
#       Copyright (C) 2004-2007 Alex Marandon
# 
#   This file is part of slag, a pattern-based audio sequencer.
# 
#   slag is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2, or (at your option)
#   any later version.
# 
#   slag is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
# 
#   You should have received a copy of the GNU General Public License
#   along with slag; see the file COPYING.  If not, write to
#   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
#
########################################################################

# Process this file with autoconf to produce a configure script.
AC_INIT(Slag, 0.1, al@alpage.org, slag)

AC_CONFIG_SRCDIR(src/config.h.in)
AC_CONFIG_HEADERS(src/config.h)
AC_CONFIG_FILES(Makefile slag.pro)

# Set default installation prefix.
AC_PREFIX_DEFAULT(/usr/local)
ac_prefix=$prefix
if test "x$ac_prefix" = "xNONE"; then
    ac_prefix=$ac_default_prefix
fi
AC_SUBST(ac_prefix)
AC_DEFINE_UNQUOTED(CONFIG_PREFIX, ["$ac_prefix"], 
                   [Default installation prefix.])

# Enable debugging argument option.
AC_ARG_ENABLE(debug,
  AC_HELP_STRING([--enable-debug], [enable debugging (default=no)]),
  [ac_debug="debug"],
  [ac_debug="release"])
AC_SUBST(ac_debug)

if test "x$ac_debug" = "xdebug"; then
   AC_DEFINE(CONFIG_DEBUG, 1, [Define if debugging is enabled.])
fi

# Use double to represent samples internally.
#AC_ARG_ENABLE(double,
  #AC_HELP_STRING([--enable-double], 
     #[enable double samples representation (default=no)]),
     #[ac_double="yes"],
     #[ac_double="no"])
#
#if test "x$ac_double" = "xyes"; then
    #AC_DEFINE(USE_DOUBLE, 1, [Define if double is enabled.])
#fi

# Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_GCC_TRADITIONAL

# Checks for languages.
AC_LANG_C
AC_LANG_CPLUSPLUS

# Prepend alternate dependencies paths.
for X in $ac_with_paths; do
  if test -d $X/include; then
    CFLAGS="$CFLAGS -I$X/include"
    CPPFLAGS="$CPPFLAGS -I$X/include"
    ac_incpath="$ac_incpath $X/include"
  fi
  if test -d $X/lib64; then
    LIBS="$LIBS -L$X/lib64"
    ac_libs="$ac_libs -L$X/lib64"
  fi
  if test -d $X/lib; then
    LIBS="$LIBS -L$X/lib"
    ac_libs="$ac_libs -L$X/lib"
  fi
done

# Check for QTDIR environment variable.
AC_MSG_CHECKING([whether QTDIR environment variable is set])
if test "x$QTDIR" = "x"; then
  AC_MSG_RESULT([no])
  AC_MSG_ERROR([QTDIR must be properly set.])
else
  AC_MSG_RESULT([$QTDIR])
fi

CFLAGS="$CFLAGS -I$QTDIR/include"
CPPFLAGS="$CPPFLAGS -I$QTDIR/include"
if test -d $QTDIR/lib64; then
  LIBS="$LIBS -L$QTDIR/lib64"
else
  LIBS="$LIBS -L$QTDIR/lib"
fi
for X in qt qt3; do
  if test -d $QTDIR/include/$X; then
    CFLAGS="$CFLAGS -I$QTDIR/include/$X"
    CPPFLAGS="$CPPFLAGS -I$QTDIR/include/$X"
  fi
done

# Checks for Qt library.
#for X in qt-mt qt; do
#  if test "x$ac_qtlib" = "x"; then
#     AC_CHECK_LIB($X, main, [ac_qtlib=$X])
#  fi
#done
#if test "x$ac_qtlib" = "x"; then
#  AC_MSG_ERROR([Qt library not found. Maybe QTDIR isn't properly set.])
#fi
#AC_SUBST(ac_qtlib)

# Check for Qt multi-thread support.
#if ! test "x$ac_qtlib" = "xqt-mt"; then
#  AC_MSG_ERROR([Qt library not compiled with thread support.])
#fi

AC_CACHE_CHECK([for Qt library version >= 4.2.0],
  ac_qtlib_version, [
  AC_TRY_LINK([#include <Qt/qglobal.h>], [
#if QT_VERSION < 0x040200
#error Qt library 4.2.0 or greater required.
#endif
    ],
    ac_qtlib_version="yes", [
      echo "no; Qt 4.2.0 or greater is required"
      exit
    ])
])

# A common error message:
ac_qtdir_errmsg="not found in current PATH. Maybe Qt development environment isn't available (qt3-devel)."

# Check for Qt qmake utility.
AC_PATH_PROG(ac_qmake, qmake, [no], $QTDIR/bin:${PATH})
if test "x$ac_qmake" = "xno"; then
   AC_MSG_ERROR([qmake $ac_qtdir_errmsg])
fi
AC_SUBST(ac_qmake)

# Check for Qt moc utility.
AC_PATH_PROG(ac_moc, moc, [no], $QTDIR/bin:${PATH})
if test "x$ac_moc" = "xno"; then
   AC_MSG_ERROR([moc $ac_qtdir_errmsg])
fi
AC_SUBST(ac_moc)

# Check for Qt uic utility.
AC_PATH_PROG(ac_uic, uic, [no], $QTDIR/bin:${PATH})
if test "x$ac_uic" = "xno"; then
   AC_MSG_ERROR([uic $ac_qtdir_errmsg])
fi
AC_SUBST(ac_uic)

# Checks for libraries.
AC_CHECK_LIB(m, main)
#AC_CHECK_LIB(X11, main)
#AC_CHECK_LIB(Xext, main)
AC_CHECK_LIB($ac_qtlib, main)
AC_CHECK_LIB([pthread], [main])

### check libsndfile presence
PKG_CHECK_MODULES(SNDFILE, sndfile >= 1.0.11)
# make substitutions in slag.pro.in
AC_SUBST(SNDFILE_CFLAGS)
AC_SUBST(SNDFILE_LIBS)

PKG_CHECK_MODULES(SAMPLERATE, samplerate >= 0.1.2)
AC_SUBST(SAMPLERATE_CFLAGS)
AC_SUBST(SAMPLERATE_LIBS)


# testing the presence of libjack
jack_support="no"

MY_PKG_CONFIG_MSG="
Consider adjusting the PKG_CONFIG_PATH environment variable if you
installed software in a non-standard prefix.
"

PKG_CHECK_MODULES(JACK, jack >= 0.100.0, 
                  ac_jack_version="yes", ac_jack_version="no")


if ! test "x$ac_jack_version" = "xyes"; then
        AC_MSG_RESULT()
        AC_MSG_RESULT(** $JACK_PKG_ERRORS)
        AC_MSG_RESULT(** JACK can be fetched from http://jackit.sf.net)
        AC_MSG_RESULT($MY_PKG_CONFIG_MSG)
else
        AC_SUBST(JACK_CFLAGS)
        AC_SUBST(JACK_LIBS)
        ac_audio_output="$ac_audio_output jack"
        jack_support="yes"
fi

# testing the presence of libao

libao_support="no"

PKG_CHECK_MODULES(AO, ao >= 0.8.5, ac_ao_version="yes", ac_ao_version="no")
if ! test "x$ac_ao_version" = "xyes"; then
        AC_MSG_RESULT()
        AC_MSG_RESULT(** $AO_PKG_ERRORS)
        AC_MSG_RESULT(** libao can be fetched from http://xiph.org/ao)
        AC_MSG_RESULT($MY_PKG_CONFIG_MSG)
else
        AC_SUBST(AO_CFLAGS)
        AC_SUBST(AO_LIBS)
        ac_audio_output="$ac_audio_output libao"
        libao_support="yes"
fi

AC_SUBST(ac_libs)
AC_SUBST(ac_incpath)
AC_SUBST(ac_audio_output)

# Checks for header files.
AC_CHECK_HEADERS([unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE

# Checks for library functions.
AC_HEADER_STDC
AC_CHECK_FUNCS([memset])

# Finally produce a configure header file and the makefiles.
AC_OUTPUT

#make clean > /dev/null 2>&1

echo ""
echo "libao support : $libao_support."
echo "JACK support : $jack_support."
echo ""
echo "Now type 'make', followed by 'make install' as root."
echo ""
