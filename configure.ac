# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([UnitTestris],[0.1],[ajrich@earthlink.net])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_CHECK_PROG(HAVE_FLUID,fluid,yes)
if test x"${HAVE_FLUID}" != x"yes" ; then
   AC_MSG_ERROR([Cannot compile without fluid])
fi
AC_CHECK_PROG(HAVE_FLTK_CONFIG,fltk-config,yes)
if test x"${HAVE_FLTK_CONFIG}" != x"yes" ; then
   AC_MSG_ERROR([Cannot compile without fltk-config])
fi
#AC_PROG_CC
#AC_PROG_INSTALL

# Checks for typedefs, structures, and compiler characteristics.

AC_HEADER_STDBOOL
AC_C_INLINE

# Get fltk compile & link flags
AC_SUBST(FLTK_CXXFLAGS,[`fltk-config --use-gl --cxxflags`])
AC_SUBST(FLTK_LDFLAGS,[`fltk-config --use-gl --ldflags`])
#AM settings

AM_INIT_AUTOMAKE([-Wall -Werror])
AM_PATH_CPPUNIT(1.12.1)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
