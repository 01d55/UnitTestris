dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([UnitTestris],[0.1],[arrlaari@gmail.com])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS([config.h])

dnl Checks for programs.
AC_PROG_CXX

AC_CHECK_PROG(HAVE_FLUID,fluid,yes)
AS_IF([test x"${HAVE_FLUID}" != x"yes" ],
[ AC_MSG_ERROR( [Cannot compile without fluid] ) ])

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

dnl For now, error out if C++11 isn't available
AS_IF([test "ac_cv_cxx_compile_cxx0x_native" != "yes" && test x"${CXX11FLAG}" = x""],
[AC_MSG_ERROR([Cannot compile without c++11 features in the compiler])])

dnl Check for packages

PKG_CHECK_MODULES([SDL], [sdl gl openal vorbisfile])
dnl FLTK as of 12/1/2013 -lfltk_gl doesn't work, need -lGL
PKG_CHECK_MODULES([GL], [gl])

dnl Get fltk compile & link flags
AC_SUBST(FLTK_CXXFLAGS,[`fltk-config --use-gl --cxxflags`])
AC_SUBST(FLTK_LDADD,[`fltk-config --use-gl --ldflags`])

dnl Checks for library functions.
AC_CHECK_FUNCS([sqrt])


dnl AM settings

AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects])
AM_PATH_CPPUNIT(1.12.1)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
