/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2014 Amrith Nayak <amrith92@gmail.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __GST_HLSSINK_H__
#define __GST_HLSSINK_H__

#include <gst/gst.h>
#include "gstm3u8playlist.h"

G_BEGIN_DECLS

/* #defines don't like whitespacey bits */
#define GST_TYPE_HLS_SINK \
  (gst_hls_sink_get_type())
#define GST_HLS_SINK(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_HLS_SINK,GstHlsSink))
#define GST_HLS_SINK_CAST(obj) \
    ((GstHlsSink *) obj)
#define GST_HLS_SINK_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_HLS_SINK,GstHlsSinkClass))
#define GST_IS_HLS_SINK(obj) \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_HLS_SINK))
#define GST_IS_HLS_SINK_CLASS(obj) \
    (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_HLS_SINK))

typedef struct _GstHlsSink      GstHlsSink;
typedef struct _GstHlsSinkClass GstHlsSinkClass;

struct _GstHlsSink
{
    GstBin bin;

    GstPad *ghostpad;
    GstElement *multifilesink;
    gboolean elements_created;
    GstEvent *force_key_unit_event;

    gchar *location;
    gchar *playlist_location;
    gchar *playlist_root;
    guint playlist_length;
    GstM3U8Playlist *playlist;
    guint index;
    gint max_files;
    gint target_duration;
    gint count;
    guint timeout_id;
    GstSegment segment;
    gboolean waiting_fku;
    GstClockTime last_running_time;
};

struct _GstHlsSinkClass 
{
  GstBinClass bin_class;
};

GType gst_hls_sink_get_type (void);
gboolean gst_hls_sink_plugin_init (GstPlugin * plugin);

G_END_DECLS

#endif /* __GST_HLSSINK_H__ */
