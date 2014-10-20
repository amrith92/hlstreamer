# Note: plugindir is set in configure

##############################################################################
# TODO: change libhlstgm.la to something else, e.g. libmysomething.la     #
##############################################################################
plugin_LTLIBRARIES = libhlstgm.la

##############################################################################
# TODO: for the next set of variables, name the prefix if you named the .la, #
#  e.g. libmysomething.la => libmysomething_la_SOURCES                       #
#                            libmysomething_la_CFLAGS                        #
#                            libmysomething_la_LIBADD                        #
#                            libmysomething_la_LDFLAGS                       #
##############################################################################

# sources used to compile this plug-in
libhlstgm_la_SOURCES = \
		m3u8.c \
		gsthlsdemux.c \
		gsttgmhlsplugin.c \
		gstm3u8playlist.c \
		gsthlssink.c

# compiler and linker flags used to compile this plugin, set in configure.ac
libhlstgm_la_CFLAGS = $(GST_PLUGINS_BAD_CFLAGS) $(GST_PLUGINS_BASE_CFLAGS) $(GST_BASE_CFLAGS) $(GST_CFLAGS) $(GIO_CFLAGS) $(LIBGCRYPT_CFLAGS) $(NETTLE_CFLAGS)
libhlstgm_la_LIBADD = \
		$(GST_PLUGINS_BAD_LIBS) \
		$(GST_PLUGINS_BASE_LIBS) \
		~/gst/master/gst-plugins-bad/gst-libs/gst/uridownloader/libgsturidownloader-1.0.la \
		-lgstpbutils-1.0 -lgstvideo-1.0 \
		$(GST_BASE_LIBS) $(GST_LIBS) $(GIO_LIBS) $(LIBM) \
		$(LIBGCRYPT_LIBS) $(NETTLE_LIBS) $(OPENSSL_LIBS)
libhlstgm_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS) -no-undefined
libhlstgm_la_LIBTOOLFLAGS = --tag=disable-static

# headers we need but don't want installed
noinst_HEADERS = \
		gsthlsdemux.h \
		m3u8.h \
		gsthlssink.h \
		gstm3u8playlist.h
