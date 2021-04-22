dnl
dnl $Id$
dnl

AC_MSG_CHECKING(for GUI support)


PHP_ARG_ENABLE(gui,,
[  --enable-gui            Enable building of the gui SAPI executable], no, no)

if test "$PHP_GUI" != "no"; then


if test -z "$PKG_CONFIG"; then
  AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
fi

if test -x "$PKG_CONFIG"; then
  dnl using pkg-config output

  AC_MSG_CHECKING(for libxml-2.0 here i am)
  if test "$PHP_GUI" = "yes" -o "$PHP_GUI" = "/usr"; then
    PKNAME=libxml-2.0
    AC_MSG_RESULT(using default path)
  elif test -r $PHP_GUI/$PHP_LIBDIR/pkgconfig/libxml-2.0.pc; then
    PKNAME=$PHP_GUI/$PHP_LIBDIR/pkgconfig/libxml-2.0.pc
    AC_MSG_RESULT(using $PKNAME)
  elif test -r $PHP_GUI/lib/pkgconfig/libxml-2.0.pc; then
    PKNAME=$PHP_GUI/lib/pkgconfig/libxml-2.0.pc
    AC_MSG_RESULT(using $PKNAME)
  else
    AC_MSG_RESULT(not found)
    AC_MSG_WARN(Could not find libxml-2.0.pc. Try without $PHP_GUI or set PKG_CONFIG_PATH)
  fi
fi

if test -n "$PKNAME"; then
  AC_MSG_CHECKING(for libxml+ 2.22.30 or greater)
  if $PKG_CONFIG --atleast-version 2.0.0 $PKNAME; then
    libxml_version_full=`$PKG_CONFIG --modversion $PKNAME`
    AC_MSG_RESULT($libxml_version_full)
  else
    AC_MSG_ERROR(libxml version 2.0.0 or later is required to compile php with GTK+PHP support)
  fi

  LIBXML_LIBS=`$PKG_CONFIG --libs   $PKNAME`
  LIBXML_INCL=`$PKG_CONFIG --cflags $PKNAME`
fi

PHP_EVAL_LIBLINE($LIBXML_LIBS, LIBXML_SHARED_LIBADD)
PHP_EVAL_INCLINE($LIBXML_INCL, LIBXML_SHARED_LIBADD)

  PHP_ADD_MAKEFILE_FRAGMENT($abs_srcdir/sapi/gui/Makefile.frag,$abs_srcdir/sapi/gui,sapi/gui)
  SAPI_GUI_PATH=sapi/gui/php
  PHP_SELECT_SAPI(gui, program, php_gui.c php_gui_parser.c php_gui_builder.c, "", '$(SAPI_GUI_PATH)')
dnl   case $host_alias in
dnl   *darwin*)
dnl     BUILD_GUI="\$(CC) \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(NATIVE_RPATHS) \$(PHP_GLOBAL_OBJS:.lo=.o) \$(PHP_BINARY_OBJS:.lo=.o) \$(PHP_GUI_OBJS:.lo=.o) \$(PHP_FRAMEWORKS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_GUI_PATH)"
dnl     ;;
dnl   *cygwin*)
dnl     SAPI_GUI_PATH=sapi/gui/php.exe
dnl     BUILD_GUI="\$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(PHP_RPATHS) \$(PHP_GLOBAL_OBJS) \$(PHP_BINARY_OBJS) \$(PHP_GUI_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_GUI_PATH)"
dnl     ;;
dnl   *)
dnl     BUILD_GUI="\$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(PHP_RPATHS) \$(PHP_GLOBAL_OBJS) \$(PHP_BINARY_OBJS) \$(PHP_GUI_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_GUI_PATH)"
dnl     ;;
dnl   esac
  BUILD_GUI="\$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(PHP_RPATHS) \$(PHP_GLOBAL_OBJS) \$(PHP_BINARY_OBJS) \$(PHP_GUI_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_GUI_PATH)"

  PHP_SUBST(SAPI_GUI_PATH)
  PHP_SUBST(BUILD_GUI)
fi

AC_MSG_RESULT($PHP_GUI)

