dnl required version of autoconf
AC_PREREQ([2.53])

dnl TODO: fill in your package name and package version here
AC_INIT([tgm-hls],[1.0.0])

dnl required versions of gstreamer and plugins-base
GST_REQUIRED=1.0.0
GSTPB_REQUIRED=1.0.0

AC_CONFIG_SRCDIR([src/gsttgmhlsplugin.c])
AC_CONFIG_HEADERS([config.h])

dnl required version of automake
AM_INIT_AUTOMAKE([1.10])

dnl enable mainainer mode by default
AM_MAINTAINER_MODE([enable])

dnl check for tools (compiler etc.)
AC_PROG_CC

dnl required version of libtool
LT_PREREQ([2.2.6])
LT_INIT

dnl give error and exit if we don't have pkgconfig
AC_CHECK_PROG(HAVE_PKGCONFIG, pkg-config, [ ], [
  AC_MSG_ERROR([You need to have pkg-config installed!])
])

dnl Check for the required version of GStreamer core (and gst-plugins-base)
dnl This will export GST_CFLAGS and GST_LIBS variables for use in Makefile.am
dnl
dnl If you need libraries from gst-plugins-base here, also add:
dnl for libgstaudio-1.0: gstreamer-audio-1.0 >= $GST_REQUIRED
dnl for libgstvideo-1.0: gstreamer-video-1.0 >= $GST_REQUIRED
dnl for libgsttag-1.0: gstreamer-tag-1.0 >= $GST_REQUIRED
dnl for libgstpbutils-1.0: gstreamer-pbutils-1.0 >= $GST_REQUIRED
dnl for libgstfft-1.0: gstreamer-fft-1.0 >= $GST_REQUIRED
dnl for libgstinterfaces-1.0: gstreamer-interfaces-1.0 >= $GST_REQUIRED
dnl for libgstrtp-1.0: gstreamer-rtp-1.0 >= $GST_REQUIRED
dnl for libgstrtsp-1.0: gstreamer-rtsp-1.0 >= $GST_REQUIRED
dnl etc.
PKG_CHECK_MODULES(GST, [
  gstreamer-1.0 >= $GST_REQUIRED
  gstreamer-controller-1.0 >= $GST_REQUIRED
], [
  AC_SUBST(GST_CFLAGS)
  AC_SUBST(GST_LIBS)
], [
  AC_MSG_ERROR([
      You need to install or upgrade the GStreamer development
      packages on your system. On debian-based systems these are
      libgstreamer1.0-dev and libgstreamer-plugins-base1.0-dev.
      on RPM-based systems gstreamer1.0-devel, libgstreamer1.0-devel
      or similar. The minimum version required is $GST_REQUIRED.
  ])
])

PKG_CHECK_MODULES(GST_BASE, [
  gstreamer-base-1.0 >= $GST_REQUIRED
], [
  AC_SUBST(GST_BASE_CFLAGS)
  AC_SUBST(GST_BASE_LIBS)
], [
  AC_MSG_ERROR([
    You need to install or upgrade the GStreamer base library
    development packages on your system.
  ])
])

PKG_CHECK_MODULES(GST_PLUGINS_BASE, [
  gstreamer-plugins-base-1.0 >= $GST_REQUIRED
], [
  AC_SUBST(GST_PLUGINS_BASE_CFLAGS)
  AC_SUBST(GST_PLUGINS_BASE_LIBS)
], [
  AC_MSG_ERROR([
    You need to install or upgrade the GStreamer base plugins
    development packages on your system.
  ])
])

PKG_CHECK_MODULES(GST_PLUGINS_GOOD, [
  gstreamer-plugins-good-1.0 >= $GST_REQUIRED
], [
  AC_SUBST(GST_PLUGINS_GOOD_CFLAGS)
  AC_SUBST(GST_PLUGINS_GOOD_LIBS)
], [
  AC_MSG_ERROR([
    You need to install or upgrade the GStreamer good plugins
    development packages on your system.
  ])
])

PKG_CHECK_MODULES(GST_PLUGINS_BAD, [
  gstreamer-plugins-bad-1.0 >= $GST_REQUIRED
], [
  AC_SUBST(GST_PLUGINS_BAD_CFLAGS)
  AC_SUBST(GST_PLUGINS_BAD_LIBS)
], [
  AC_MSG_ERROR([
    You need to install or upgrade the GStreamer bad plugins
    development packages on your system.
  ])
])

PKG_CHECK_MODULES(GIO, [
    gio-2.0 >= 2.0
], [
    AC_SUBST(GIO_CFLAGS)
    AC_SUBST(GIO_LIBS)
], [
    AC_MSG_ERROR([
        Looks like you are missing the required GIO
        libraries. The minimum version required is 2.0.
    ])
])

PKG_CHECK_MODULES(NETTLE, nettle,
    [
        AC_DEFINE(HAVE_NETTLE, 1, [Define if nettle is available])
    ], [
        AM_PATH_LIBGCRYPT([1.2.0],
        [
            AC_DEFINE(HAVE_LIBGCRYPT, 1, [Define if libgcrypt is available])
        ], [
            PKG_CHECK_MODULES(OPENSSL, openssl,
            [
                AC_DEFINE(HAVE_OPENSSL, 1, [Define if openssl is available])
            ])
        ])
    ]
)


dnl check if compiler understands -Wall (if yes, add -Wall to GST_CFLAGS)
AC_MSG_CHECKING([to see if compiler understands -Wall])
save_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -Wall"
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([ ], [ ])], [
  GST_CFLAGS="$GST_CFLAGS -Wall"
  AC_MSG_RESULT([yes])
], [
  AC_MSG_RESULT([no])
])

dnl set the plugindir where plugins should be installed (for src/Makefile.am)
if test "x${prefix}" = "x$HOME"; then
  plugindir="$HOME/.gstreamer-1.0/plugins"
else
  plugindir="\$(libdir)/gstreamer-1.0"
fi
AC_SUBST(plugindir)

dnl set proper LDFLAGS for plugins
GST_PLUGIN_LDFLAGS='-module -avoid-version -export-symbols-regex [_]*\(gst_\|Gst\|GST_\).*'
AC_SUBST(GST_PLUGIN_LDFLAGS)

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT

