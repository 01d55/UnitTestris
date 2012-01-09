# SYNOPSIS
#
#   AX_CXX_USE_STDCXX_11
#
# DESCRIPTION
#
#   If the compiler requires a flag to use C++11 features, set CXX11FLAG.
#   Depends on AX_CXX_COMPILE_STDCXX_0X
#
# LICENSE
#
#   Copyright (c) 2012 Alan Richardson <ajrich@earthlink.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

AC_DEFUN([AX_CXX_USE_STDCXX_11],
[
  if test "$ax_cv_cxx_compile_cxx0x_native" = no &&
     test "$ax_cv_cxx_compile_cxx0x_cxx" = yes; then
     AC_SUBST(CXX11FLAG,[-std=c++0x])
  fi

  if test "$ax_cv_cxx_compile_cxx0x_native" = no &&
     test "$ax_cv_cxx_compile_cxx0x_cxx" = no &&
     test "$ax_cv_cxx_compile_cxx0x_gxx" = yes ; then
     AC_SUBST(CXX11FLAG,[-std=gnu++0x])
  fi
])
