dnl This file is includes into configure.in and modules.in

dnl Get configuration directory
AC_PROG_MAKE_SET
AC_PREFIX_DEFAULT(/usr/local/dicom)
common_makefile=Makefile.def
AC_SUBST(common_makefile)

