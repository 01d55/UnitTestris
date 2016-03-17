# SYNOPSIS
#
#   AX_CXX_COMPILE_STDCXX_11_NULLPTR
#
# DESCRIPTION
#
#   Check for C++11 nullptr symbol suport in the compiler.
#
# LICENSE
#
#   Copyright (c) 2008 Benjamin Kosnik <bkoz@redhat.com>, 2011 Alan Richardson 
#   <arrlaari@gmail.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

AC_DEFUN([AX_CXX_COMPILE_STDCXX_11_NULLPTR],
[
 AC_CACHE_CHECK(if g++ supports C++11 nullptr without additional flags,
  ax_cv_cxx_compile_cxx11_nullptr_native,
  [AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  AC_TRY_COMPILE([
    char *mptr = nullptr;],,
  ax_cv_cxx_compile_cxx11_nullptr_native=yes, ax_cv_cxx_compile_cxx11_nullptr_native=no)
  AC_LANG_RESTORE
  ])

  AC_CACHE_CHECK(if g++ supports C++11 nullptr with -std=c++0x,
  ax_cv_cxx_compile_cxx11_nullptr_cxx,
  [AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  ac_save_CXXFLAGS="$CXXFLAGS"
  CXXFLAGS="$CXXFLAGS -std=c++0x"
  AC_TRY_COMPILE([
    char *mptr = nullptr;],,
  ax_cv_cxx_compile_cxx11_nullptr_cxx=yes, ax_cv_cxx_compile_cxx11_nullptr_cxx=no)
  CXXFLAGS="$ac_save_CXXFLAGS"
  AC_LANG_RESTORE
  ])

  AC_CACHE_CHECK(if g++ supports C++11 nullptr with -std=gnu++0x,
  ax_cv_cxx_compile_cxx11_nullptr_gxx,
  [AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  ac_save_CXXFLAGS="$CXXFLAGS"
  CXXFLAGS="$CXXFLAGS -std=gnu++0x"
  AC_TRY_COMPILE([
    char * mptr = nullptr;],,
  ax_cv_cxx_compile_cxx11_nullptr_gxx=yes, ax_cv_cxx_compile_cxx11_nullptr_gxx=no)
  CXXFLAGS="$ac_save_CXXFLAGS"
  AC_LANG_RESTORE
  ])

  if test "$ax_cv_cxx_compile_cxx11_nullptr_native" = yes ||
     test "$ax_cv_cxx_compile_cxx11_nullptr_cxx" = yes ||
     test "$ax_cv_cxx_compile_cxx11_nullptr_gxx" = yes; then
    AC_DEFINE(HAVE_STDCXX_11_NULLPTR,,[Define if g++ supports C++11 nullptr. ])
  fi
])
