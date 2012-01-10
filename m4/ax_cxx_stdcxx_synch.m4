# SYNOPSIS
#
#   AX_CXX_STDCXX_SYNCH
#
# DESCRIPTION
#
#   If the three major synchronization headers (<atomic>,<chrono>,<thread>) are all
#   present, define HAVE_STDCXX_SYNCH. Must call AX_CXX_USE_STDCXX_11
#
# LICENSE
#
#   Copyright (c) 2012 Alan Richardson <ajrich@earthlink.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

AC_DEFUN([AX_CXX_STDCXX_SYNCH],
[
  ac_save_CPPFLAGS="$CPPFLAGS"
  CPPFLAGS="$CPPFLAGS $CXX11FLAG"
  AC_CHECK_HEADERS([atomic chrono thread])
  CPPFLAGS="$ac_save_CPPFLAGS"

  if test "$ac_cv_header_atomic" = yes &&
     test "$ac_cv_header_chrono" = yes &&
     test "$ac_cv_header_thread" = yes ; then
    AC_DEFINE(HAVE_STDCXX_SYNCH,,[Define if all C++11 synchronization headers are present.])
  fi

])
