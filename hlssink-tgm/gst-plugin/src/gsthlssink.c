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

/**
 * SECTION:element-hlssink
 *
 * HTTP Live Streaming sink
 *
 * <refsect2>
 * <title>Launch line</title>
 * |[
 * gst-launch-1.0 videotestsrc is-live=true ! x264enc ! mpegtsmux ! tgm-hls max-files=5
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>

#include "gsthlssink.h"
#include <gst/pbutils/pbutils.h>
#include <gst/video/video.h>
#include <gio/gio.h>
#include <glib/gstdio.h>
#include <memory.h>

GST_DEBUG_CATEGORY_STATIC (gst_hls_sink_debug);
#define GST_CAT_DEFAULT gst_hls_sink_debug

#define DEFAULT_LOCATION "segment%05d.ts"
#define DEFAULT_PLAYLIST_LOCATION "playlist.m3u8"
#define DEFAULT_PLAYLIST_ROOT NULL
#define DEFAULT_MAX_FILES 10
#define DEFAULT_TARGET_DURATION 15
#define DEFAULT_PLAYLIST_LENGTH 5

enum
{
  PROP_0,
  PROP_LOCATION,
  PROP_PLAYLIST_LOCATION,
  PROP_PLAYLIST_ROOT,
  PROP_MAX_FILES,
  PROP_TARGET_DURATION,
  PROP_PLAYLIST_LENGTH
};

/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS_ANY);

#define gst_hls_sink_parent_class parent_class
G_DEFINE_TYPE (GstHlsSink, gst_hls_sink, GST_TYPE_BIN);

static void gst_hls_sink_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_hls_sink_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);
static void gst_hls_sink_handle_message (GstBin * bin, GstMessage * message);
static GstPadProbeReturn gst_hls_sink_ghost_event_probe (GstPad * pad,
        GstPadProbeInfo * info, gpointer data);
static GstPadProbeReturn gst_hls_sink_ghost_buffer_probe (GstPad * pad,
        GstPadProbeInfo * info, gpointer data);
static void gst_hls_sink_reset (GstHlsSink * sink);
static GstStateChangeReturn
gst_hls_sink_change_state (GstElement * element, GstStateChange trans);
static gboolean schedule_next_key_unit (GstHlsSink * sink);


/* GObject vmethod implementations */
static void
gst_hls_sink_dispose (GObject * object)
{
    GstHlsSink *sink = GST_HLS_SINK_CAST (object);

    G_OBJECT_CLASS (parent_class)->dispose ((GObject *) sink);
}

static void
gst_hls_sink_finalize (GObject * object)
{
    GstHlsSink *sink = GST_HLS_SINK_CAST (object);

    g_free (sink->location);
    g_free (sink->playlist_location);
    g_free (sink->playlist_root);
    if (sink->playlist) {
        gst_m3u8_playlist_free (sink->playlist);
    }

    G_OBJECT_CLASS (parent_class)->finalize ((GObject *) sink);
}

/* initialize the hlssink's class */
static void
gst_hls_sink_class_init (GstHlsSinkClass * klass)
{
    GObjectClass *gobject_class;
    GstElementClass *gstelement_class;
    GstBinClass *bin_class;

    gobject_class = (GObjectClass *) klass;
    gstelement_class = (GstElementClass *) klass;
    bin_class = (GstBinClass *) klass;

    gst_element_class_set_static_metadata(gstelement_class,
            "HTTP Live Streaming Server bin",
            "Sink",
            "HTTP Live Streaming Server bin",
            "Amrith Nayak <amrith92@gmail.com>");

    gst_element_class_add_pad_template (gstelement_class,
            gst_static_pad_template_get (&sink_factory));

    gstelement_class->change_state = GST_DEBUG_FUNCPTR (gst_hls_sink_change_state);

    bin_class->handle_message = gst_hls_sink_handle_message;

    gobject_class->dispose = gst_hls_sink_dispose;
    gobject_class->finalize = gst_hls_sink_finalize;
    gobject_class->set_property = gst_hls_sink_set_property;
    gobject_class->get_property = gst_hls_sink_get_property;

    g_object_class_install_property (gobject_class, PROP_LOCATION,
            g_param_spec_string ("location", "File Location",
                "Location of the file to write", DEFAULT_LOCATION,
                G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
    g_object_class_install_property (gobject_class, PROP_PLAYLIST_LOCATION,
            g_param_spec_string ("playlist-location", "Playlist Location",
                "Location of the playlist to write", DEFAULT_PLAYLIST_LOCATION,
                G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
    g_object_class_install_property (gobject_class, PROP_PLAYLIST_ROOT,
            g_param_spec_string ("playlist-root", "Playlist Root",
                "Location of the playlist root to write", DEFAULT_PLAYLIST_ROOT,
                G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
    g_object_class_install_property (gobject_class, PROP_MAX_FILES,
            g_param_spec_uint ("max-files", "Max Files",
                "Maximum number of files to keep on disk. Once the maximum is reached,"
                "old files start to be deleted to make room for newer ones.",
                0, G_MAXUINT, DEFAULT_MAX_FILES,
                G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
    g_object_class_install_property (gobject_class, PROP_TARGET_DURATION,
            g_param_spec_uint ("target-duration", "Target duration",
                "The target duration in seconds of a segment. "
                "(0 - disabled, useful for management of segment duration by the "
                "streaming server)",
                0, G_MAXUINT, DEFAULT_TARGET_DURATION,
                G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
    g_object_class_install_property (gobject_class, PROP_PLAYLIST_LENGTH,
            g_param_spec_uint ("playlist-length", "Playlist Length",
                "Length of the HLS playlist. To allow players to conform to section 6.3.3 "
                "of the HLS spec, this should be at least 3.",
                1, G_MAXUINT, DEFAULT_PLAYLIST_LENGTH,
                G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void
gst_hls_sink_init (GstHlsSink * sink)
{
    GstPadTemplate *templ = gst_static_pad_template_get (&sink_factory);
    sink->ghostpad = gst_ghost_pad_new_no_target_from_template ("sink", templ);
    gst_object_unref (templ);
    gst_element_add_pad (GST_ELEMENT_CAST (sink), sink->ghostpad);
    gst_pad_add_probe (sink->ghostpad, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM,
            gst_hls_sink_ghost_event_probe, sink, NULL);
    gst_pad_add_probe (sink->ghostpad, GST_PAD_PROBE_TYPE_BUFFER,
            gst_hls_sink_ghost_buffer_probe, sink, NULL);

    sink->location = g_strdup (DEFAULT_LOCATION);
    sink->playlist_location = g_strdup (DEFAULT_PLAYLIST_LOCATION);
    sink->playlist_root = g_strdup (DEFAULT_PLAYLIST_ROOT);
    sink->playlist_length = DEFAULT_PLAYLIST_LENGTH;
    sink->max_files = DEFAULT_MAX_FILES;
    sink->target_duration = DEFAULT_TARGET_DURATION;

    GST_OBJECT_FLAG_SET (sink, GST_ELEMENT_FLAG_SINK);

    gst_hls_sink_reset (sink);
}

static void
gst_hls_sink_reset (GstHlsSink * sink)
{
    sink->index = 0;
    sink->count = 0;
    sink->timeout_id = 0;
    sink->last_running_time = 0;
    sink->waiting_fku = FALSE;
    gst_event_replace (&sink->force_key_unit_event, NULL);
    gst_segment_init (&sink->segment, GST_FORMAT_UNDEFINED);

    if (sink->playlist) {
        gst_m3u8_playlist_free (sink->playlist);
    }

    sink->playlist = gst_m3u8_playlist_new (6, sink->playlist_length, FALSE);
}

static gboolean
gst_hls_sink_create_elements (GstHlsSink * sink)
{
    GstPad *pad = NULL;

    GST_DEBUG_OBJECT (sink, "Creating internal elements");

    if (sink->elements_created) {
        return TRUE;
    }

    sink->multifilesink = gst_element_factory_make ("multifilesink", NULL);
    if (sink->multifilesink == NULL) {
        goto missing_element;
    }

    g_object_set (sink->multifilesink, "location", sink->location,
            "next-file", 3, "post-messages", TRUE, "max-files", sink->max_files,
            NULL);

    gst_bin_add (GST_BIN_CAST (sink), sink->multifilesink);

    pad = gst_element_get_static_pad (sink->multifilesink, "sink");
    gst_ghost_pad_set_target (GST_GHOST_PAD (sink->ghostpad), pad);
    gst_object_unref (pad);

    sink->elements_created = TRUE;
    return TRUE;

missing_element:
    gst_element_post_message (GST_ELEMENT_CAST (sink),
            gst_missing_element_message_new (GST_ELEMENT_CAST (sink),
                "multifilesink"));
    GST_ELEMENT_ERROR (sink, CORE, MISSING_PLUGIN,
            (("Missing element '%s' - check your GStreamer installation."),
             "multifilesink"), (NULL));
    return FALSE;
}

/* GstElement vmethod implementations */
static void
gst_hls_sink_handle_message (GstBin * bin, GstMessage * message)
{
    GstHlsSink *sink = GST_HLS_SINK_CAST (bin);

    switch (message->type) {
        case GST_MESSAGE_ELEMENT:
        {
            GFile *file;
            const char *filename, *title;
            char *playlist_content;
            GstClockTime running_time, duration;
            gboolean discontinuous = FALSE;
            GError *error = NULL;
            gchar *entry_location;
            const GstStructure *structure;

            structure = gst_message_get_structure (message);
            if (strcmp (gst_structure_get_name (structure), "GstMultiFileSink")) {
                break;
            }

            filename = gst_structure_get_string (structure, "filename");
            gst_structure_get_clock_time (structure, "running-time", &running_time);
            duration = running_time - sink->last_running_time;
            sink->last_running_time = running_time;

            file = g_file_new_for_path (filename);
            title = "playlist";
            GST_INFO_OBJECT (sink, "COUNT %d", sink->index);
            if (sink->playlist_root == NULL) {
                entry_location = g_path_get_basename (filename);
            } else {
                gchar *name = g_path_get_basename (filename);
                entry_location = g_build_filename (sink->playlist_root, name, NULL);
                g_free (name);
            }

            gst_m3u8_playlist_add_entry (sink->playlist, entry_location, file,
                    title, duration, sink->index, discontinuous);
            g_free (entry_location);
            playlist_content = gst_m3u8_playlist_render (sink->playlist);
            g_file_set_contents (sink->playlist_location,
                    playlist_content, -1, &error);
            g_free (playlist_content);

            /* multifilesink is starting a new file which means that upstream sent a
             * key unit and we can schedule the next key unit now.
             */
            sink->waiting_fku = FALSE;
            schedule_next_key_unit (sink);

            GST_DEBUG_OBJECT (bin, "dropping message %" GST_PTR_FORMAT, message);
            gst_message_unref (message);
            message = NULL;
            break;
        }
        default:
            break;
    }

    if (message) {
        GST_BIN_CLASS (parent_class)->handle_message (bin, message);
    }
}

static GstStateChangeReturn
gst_hls_sink_change_state (GstElement * element, GstStateChange trans)
{
    GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
    GstHlsSink *sink = GST_HLS_SINK_CAST (element);

    switch (trans) {
        case GST_STATE_CHANGE_NULL_TO_READY:
            if (!gst_hls_sink_create_elements (sink)) {
                return GST_STATE_CHANGE_FAILURE;
            }
            break;
        case GST_STATE_CHANGE_PAUSED_TO_PLAYING:
            break;
        default:
            break;
    }

    ret = GST_ELEMENT_CLASS (parent_class)->change_state (element, trans);

    switch (trans) {
        case GST_STATE_CHANGE_PLAYING_TO_PAUSED:
            break;
        case GST_STATE_CHANGE_PAUSED_TO_READY:
            gst_hls_sink_reset (sink);
            break;
        case GST_STATE_CHANGE_READY_TO_NULL:
            gst_hls_sink_reset (sink);
            break;
        default:
            break;
    }

    return ret;
}

static void
gst_hls_sink_set_property (GObject * object, guint prop_id,
        const GValue * value, GParamSpec * pspec)
{
    GstHlsSink *sink = GST_HLS_SINK_CAST (object);

    switch (prop_id) {
        case PROP_LOCATION:
            g_free (sink->location);
            sink->location = g_value_dup_string (value);
            if (sink->multifilesink) {
                g_object_set (sink->multifilesink, "location", sink->location, NULL);
            }
            break;
        case PROP_PLAYLIST_LOCATION:
            g_free (sink->playlist_location);
            sink->playlist_location = g_value_dup_string (value);
            break;
        case PROP_PLAYLIST_ROOT:
            g_free (sink->playlist_root);
            sink->playlist_root = g_value_dup_string (value);
            break;
        case PROP_MAX_FILES:
            sink->max_files = g_value_get_uint (value);
            if (sink->multifilesink) {
                g_object_set (sink->multifilesink, "location", sink->location,
                        "next-file", 3, "post-messages", TRUE, "max-files",
                        sink->max_files, NULL);
            }
            break;
        case PROP_TARGET_DURATION:
            sink->target_duration = g_value_get_uint (value);
            break;
        case PROP_PLAYLIST_LENGTH:
            sink->playlist_length = g_value_get_uint (value);
            sink->playlist->window_size = sink->playlist_length;
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
gst_hls_sink_get_property (GObject * object, guint prop_id,
        GValue * value, GParamSpec * pspec)
{
    GstHlsSink *sink = GST_HLS_SINK_CAST (object);

    switch (prop_id) {
        case PROP_LOCATION:
            g_value_set_string (value, sink->location);
            break;
        case PROP_PLAYLIST_LOCATION:
            g_value_set_string (value, sink->playlist_location);
            break;
        case PROP_PLAYLIST_ROOT:
            g_value_set_string (value, sink->playlist_root);
            break;
        case PROP_MAX_FILES:
            g_value_set_uint (value, sink->max_files);
            break;
        case PROP_TARGET_DURATION:
            g_value_set_uint (value, sink->target_duration);
            break;
        case PROP_PLAYLIST_LENGTH:
            g_value_set_uint (value, sink->playlist_length);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static GstPadProbeReturn
gst_hls_sink_ghost_event_probe (GstPad * pad, GstPadProbeInfo * info,
        gpointer data)
{
    GstHlsSink *sink = GST_HLS_SINK_CAST (data);
    GstEvent *event = gst_pad_probe_info_get_event (info);

    switch (GST_EVENT_TYPE (event)) {
        case GST_EVENT_SEGMENT:
        {
            gst_event_copy_segment (event, &sink->segment);
            break;
        }
        case GST_EVENT_FLUSH_STOP:
            gst_segment_init (&sink->segment, GST_FORMAT_UNDEFINED);
            break;
        case GST_EVENT_CUSTOM_DOWNSTREAM:
        {
            GstClockTime timestamp;
            GstClockTime running_time, stream_time;
            gboolean all_headers;
            guint count;

            if (!gst_video_event_is_force_key_unit (event)) {
                break;
            }

            gst_event_replace (&sink->force_key_unit_event, event);
            gst_video_event_parse_downstream_force_key_unit (event,
                    &timestamp, &stream_time, &running_time,
                    &all_headers, &count);
            GST_INFO_OBJECT (sink, "setting index %d", count);
            sink->index = count;
            break;
        }
        default:
            break;
    }

    return GST_PAD_PROBE_OK;
}

static gboolean
schedule_next_key_unit (GstHlsSink * sink)
{
    gboolean res = TRUE;
    GstClockTime running_time;
    GstPad *sinkpad = gst_element_get_static_pad (GST_ELEMENT (sink), "sink");

    if (sink->target_duration == 0) {
        goto out;
    }

    running_time = sink->last_running_time + sink->target_duration * GST_SECOND;
    GST_INFO_OBJECT (sink, "sending upstream force-key-unit, index %d "
            "now %" GST_TIME_FORMAT " target %" GST_TIME_FORMAT,
            sink->index + 1, GST_TIME_ARGS (sink->last_running_time),
            GST_TIME_ARGS (running_time));

    if (!(res = gst_pad_push_event (sinkpad,
                    gst_video_event_new_upstream_force_key_unit (running_time,
                        TRUE, sink->index + 1)))) {
        GST_ERROR_OBJECT (sink, "Failed to push upstream force key unit event");
    }

out:
    sink->waiting_fku = res;
    gst_object_unref (sinkpad);
    return res;
}

static GstPadProbeReturn
gst_hls_sink_ghost_buffer_probe (GstPad * pad, GstPadProbeInfo * info,
        gpointer data)
{
    GstHlsSink *sink = GST_HLS_SINK_CAST (data);
    GstBuffer *buffer = gst_pad_probe_info_get_buffer (info);
    GstClockTime timestamp;

    timestamp = GST_BUFFER_TIMESTAMP (buffer);
    if (sink->target_duration == 0 || !GST_CLOCK_TIME_IS_VALID (timestamp)
            || sink->waiting_fku) {
        return GST_PAD_PROBE_OK;
    }

    sink->last_running_time = gst_segment_to_running_time (&sink->segment,
            GST_FORMAT_TIME, timestamp);
    schedule_next_key_unit (sink);
    return GST_PAD_PROBE_OK;
}

/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
gboolean
gst_hls_sink_plugin_init (GstPlugin * plugin)
{
  /* debug category for fltering log messages
   *
   */
  GST_DEBUG_CATEGORY_INIT (gst_hls_sink_debug, "tgm-hls",
      0, "HlsSink");

  return gst_element_register (plugin, "tgm-hls", GST_RANK_NONE,
      GST_TYPE_HLS_SINK);
}

/**
 *  Plugin definition
 */
GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    hlssink,
    "HTTP Live Streaming Server",
    gst_hls_sink_plugin_init,
    VERSION,
    "LGPL",
    "thegeekmachine",
    "http://thegeekpa.wordpress.com"
)
