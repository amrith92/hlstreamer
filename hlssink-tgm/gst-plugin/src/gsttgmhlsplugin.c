#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gst/gst.h>

#include "gsthlsdemux.h"
#include "gsthlssink.h"

GST_DEBUG_CATEGORY_STATIC (tgm_hls_demux_debug);

static gboolean
tgm_plugin_init (GstPlugin * plugin)
{
  GST_DEBUG_CATEGORY_INIT (tgm_hls_demux_debug, "tgm-hlsdemux", 0, "HlsDemux");

  if (!gst_element_register (plugin, "tgm-hlsdemux", GST_RANK_PRIMARY,
          GST_TYPE_HLS_DEMUX) || FALSE) {
    return FALSE;
  }

  if (!gst_hls_sink_plugin_init (plugin)) {
    return FALSE;
  }

  return TRUE;
}

/**
 *  Plugin definition
 */
GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    tgmhls,
    "HTTP Live Streaming Server",
    tgm_plugin_init,
    VERSION, "LGPL", "thegeekmachine", "http://thegeekpa.wordpress.com")
