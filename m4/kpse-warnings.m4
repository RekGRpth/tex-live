# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 0

# KPSE_COMPILER_WARNINGS
# ----------------------
# Set up compiler warnings for C and C++.
# This macro determines and substitutes WARNING_CFLAGS for the C compiler
# and, if applicable, WARNING_CXXFLAGS for the C++ compiler. To activate
# them a Makefile.am must use them, e.g., in AM_CFLAGS or AM_CXXFLAGS.
AC_DEFUN([KPSE_COMPILER_WARNINGS],
[AC_REQUIRE([_KPSE_COMPILER_WARNINGS_OPTION])[]dnl
dnl arrange that AC_PROG_CC uses _KPSE_WARNING_CFLAGS etc.
AC_PROVIDE_IFELSE([AC_PROG_CC],
                  [_KPSE_WARNING_CFLAGS],
                  [m4_define([AC_PROG_CC],
                             m4_defn([AC_PROG_CC])[_KPSE_WARNING_CFLAGS])])
AC_PROVIDE_IFELSE([AC_PROG_CXX],
                  [_KPSE_WARNING_CXXFLAGS],
                  [m4_define([AC_PROG_CXX],
                             m4_defn([AC_PROG_CXX])[_KPSE_WARNING_CXXFLAGS])])
AC_PROVIDE_IFELSE([AC_PROG_OBJC],
                  [_KPSE_WARNING_OBJCFLAGS],
                  [m4_define([AC_PROG_OBJC],
                             m4_defn([AC_PROG_OBJC])[_KPSE_WARNING_OBJCFLAGS])])
]) # KPSE_COMPILER_WARNINGS

# _KPSE_COMPILER_WARNINGS_OPTION
# ------------------------------
# Internal subroutine.
# Provide configure option `--enable-compiler-warnings=[no|min|yes|max|all]'
# to enable various degrees of compiler warnings.
AC_DEFUN([_KPSE_COMPILER_WARNINGS_OPTION],
[AC_ARG_ENABLE([compiler-warnings],
                AS_HELP_STRING([--enable-compiler-warnings=@<:@no|min|yes|max|all@:>@],
                               [Turn on compiler warnings @<:@default: yes if maintainer-mode,
                                min otherwise@:>@]))[]dnl
AS_CASE([$enable_compiler_warnings],
        [no | min | yes | max | all], [],
        [AS_IF([test "x$enable_maintainer_mode" = xyes],
               [enable_compiler_warnings=yes],
               [enable_compiler_warnings=min])])
]) # _KPSE_COMPILER_WARNINGS_OPTION

_KPSE_WARNING_CFLAGS
# ------------------
# Internal subroutine.
# Determine and substitute WARNING_CFLAGS for C compiler.
AC_DEFUN([_KPSE_WARNING_CFLAGS],
[AC_REQUIRE([_KPSE_COMPILER_WARNINGS_OPTION])[]dnl
AC_REQUIRE([AC_PROG_CC])[]dnl
AC_CACHE_CHECK([what warning flags to pass to the C compiler],
               [kpse_cv_warning_cflags],
               [dnl
if test "x$enable_compiler_warnings" = xno; then
  kpse_cv_warning_cflags=
elif test "x$GCC" = xyes; then
  _KPSE_WARNING_GNU_CFLAGS([CC], [cflags])[]dnl
else
  : # FIXME: warning flags for non-GNU C compilers
fi])
WARNING_CFLAGS=$kpse_cv_warning_cflags
AC_SUBST([WARNING_CFLAGS])
m4_define([_KPSE_WARNING_CFLAGS], [])[]dnl
]) # _KPSE_WARNING_CFLAGS

# _KPSE_WARNING_CXXFLAGS
# ----------------------
# Internal subroutine.
# Determine and substitute WARNING_CXXFLAGS for C++ compiler.
m4_define([_KPSE_WARNING_CXXFLAGS],
[AC_CACHE_CHECK([what warning flags to pass to the C++ compiler],
                [kpse_cv_warning_cxxflags],
                [dnl
if test "x$enable_compiler_warnings" = xno; then
  kpse_cv_warning_cxxflags=
elif test "x$GXX" = xyes; then
  _KPSE_WARNING_GNU_CXXFLAGS([CXX], [cxxflags])[]dnl
else
  : # FIXME: warning flags for non-GNU C++ compilers
fi])
WARNING_CXXFLAGS=$kpse_cv_warning_cxxflags
AC_SUBST([WARNING_CXXFLAGS])
m4_define([_KPSE_WARNING_CXXFLAGS], [])[]dnl
]) # _KPSE_WARNING_CXXFLAGS

_KPSE_WARNING_OBJCFLAGS
# ---------------------
# Internal subroutine.
# Determine and substitute WARNING_OBJCFLAGS for Objective C compiler.
AC_DEFUN([_KPSE_WARNING_OBJCFLAGS],
[AC_REQUIRE([_KPSE_COMPILER_WARNINGS_OPTION])[]dnl
AC_REQUIRE([AC_PROG_OBJC])[]dnl
AC_CACHE_CHECK([what warning flags to pass to the Objective C compiler],
               [kpse_cv_warning_objcflags],
               [dnl
if test "x$enable_compiler_warnings" = xno; then
  kpse_cv_warning_objcflags=
elif test "x$GOBJC" = xyes; then
  _KPSE_WARNING_GNU_CFLAGS([OBJC], [objcflags])[]dnl
else
  : # FIXME: warning flags for non-GNU C compilers
fi])
WARNING_OBJCFLAGS=$kpse_cv_warning_objcflags
AC_SUBST([WARNING_OBJCFLAGS])
m4_define([_KPSE_WARNING_OBJCFLAGS], [])[]dnl
]) # _KPSE_WARNING_OBJCFLAGS

# _KPSE_WARNING_GNU_CFLAGS(COMPILER, TAG)
# ---------------------------------------
# Internal subroutine.
# Determine warning flags for GNU (Objective) C compiler.
m4_define([_KPSE_WARNING_GNU_CFLAGS],
[kpse_cv_warning_$2="-Wall -Wunused"
AS_CASE([`$[]$1 -dumpversion`],
        [3.4.* | 4.*],
        [kpse_cv_warning_$2="$kpse_cv_warning_$2 -Wdeclaration-after-statement"])
AS_CASE([`$[]$1 -dumpversion`],
        [3.@<:@234@:>@.* | 4.*],
        [kpse_cv_warning_$2="$kpse_cv_warning_$2 -Wno-unknown-pragmas"])
if test "x$enable_compiler_warnings" != xmin; then
  kpse_cv_warning_$2="$kpse_cv_warning_$2 -Wmissing-prototypes -Wmissing-declarations"
  if test "x$enable_compiler_warnings" != xyes; then
    kpse_cv_warning_$2="$kpse_cv_warning_$2 -Wimplicit -Wparentheses -Wreturn-type"
    kpse_cv_warning_$2="$kpse_cv_warning_$2 -Wswitch -Wtrigraphs -Wpointer-arith"
    kpse_cv_warning_$2="$kpse_cv_warning_$2 -Wcast-qual -Wcast-align -Wwrite-strings"
    AS_CASE([`$[]$1 -dumpversion`],
            [3.4.* | 4.*],
            [kpse_cv_warning_$2="$kpse_cv_warning_$2 -Wold-style-definition"])
    if test "x$enable_compiler_warnings" != xmax; then
      kpse_cv_warning_$2="$kpse_cv_warning_$2 -Wshadow"
    fi
  fi
fi
]) # _KPSE_WARNING_GNU_CFLAGS

# _KPSE_WARNING_GNU_CXXFLAGS(COMPILER, TAG)
# -----------------------------------------
# Internal subroutine.
# Determine warning flags for GNU (Objective) C++ compiler.
m4_define([_KPSE_WARNING_GNU_CXXFLAGS],
[kpse_cv_warning_$2="-Wall -Wunused"
AS_CASE([`$[]$1 -dumpversion`],
        [3.@<:@234@:>@.* | 4.*],
        [kpse_cv_warning_$2="$kpse_cv_warning_$2 -Wno-unknown-pragmas"])
if test "x$enable_compiler_warnings" != xmin; then
  if test "x$enable_compiler_warnings" != xyes; then
    kpse_cv_warning_$2="$kpse_cv_warning_$2 -Wimplicit -Wparentheses -Wreturn-type"
    kpse_cv_warning_$2="$kpse_cv_warning_$2 -Wswitch -Wtrigraphs -Wpointer-arith"
    kpse_cv_warning_$2="$kpse_cv_warning_$2 -Wcast-qual -Wcast-align -Wwrite-strings"
  fi
    if test "x$enable_compiler_warnings" != xmax; then
      kpse_cv_warning_$2="$kpse_cv_warning_$2 -Wshadow"
    fi
fi
]) # _KPSE_WARNING_GNU_CXXFLAGS

