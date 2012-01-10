dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([UnitTestris],[0.1],[ajrich@earthlink.net])
m4_include([m4/ax_cxx_use_stdcxx_11.m4])
m4_include([m4/ax_cxx_stdcxx_synch.m4])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS([config.h])

dnl Checks for programs.
AC_PROG_CXX

AC_CHECK_PROG(HAVE_FLUID,fluid,yes)
AS_IF([test x"${HAVE_FLUID}" != x"yes" ],
[ AC_MSG_ERROR( [Cannot compile without fluid!] ) ])

AC_CHECK_PROG(HAVE_FLTK_CONFIG,fltk-config,yes)
AS_IF([test x"${HAVE_FLTK_CONFIG}" != x"yes" ],
[AC_MSG_ERROR([Cannot compile without fltk-config])])

AC_PROG_CC
  dnl AC_PROG_INSTALL
AC_LANG([C++])

dnl Checks for typedefs, structures, and compiler characteristics.

AC_HEADER_STDBOOL
AC_C_INLINE
AX_CXX_COMPILE_STDCXX_0X

dnl Custom checks for C++11 features

AX_CXX_USE_STDCXX_11
AX_CXX_STDCXX_SYNCH


dnl Get fltk compile & link flags
AC_SUBST(FLTK_CXXFLAGS,[`fltk-config --use-gl --cxxflags`])
AC_SUBST(FLTK_LDADD,[`fltk-config --use-gl --ldflags`])
dnl AM settings

AM_INIT_AUTOMAKE([-Wall -Werror])
AM_PATH_CPPUNIT(1.12.1)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
