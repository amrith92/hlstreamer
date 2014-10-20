/*
 * GStreamer
 * Copyright (C) 2010 Marc-Andre Lureau <marcandre.lureau@gmail.com>
 * Copyright (C) 2010 Andoni Morales Alastruey <ylatuya@gmail.com>
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
#ifndef __M3U8_H
#define __M3U8_H

#include <glib.h>
#include <gst/gst.h>

G_BEGIN_DECLS

typedef struct _GstM3U8 GstM3U8;
typedef struct _GstM3U8MediaFile GstM3U8MediaFile;
typedef struct _GstM3U8Client GstM3U8Client;

#define GST_M3U8(m) ((GstM3U8 *) m)
#define GST_M3U8_MEDIA_FILE(f) ((GstM3U8MediaFile *) f)

#define GST_M3U8_CLIENT_LOCK(c) g_mutex_lock (&c->lock);
#define GST_M3U8_CLIENT_UNLOCK(c) g_mutex_unlock (&c->lock);

#define GST_M3U8_CLIENT_IS_LIVE(c) ((! (c)->current || (c)->current->endlist) ? FALSE : TRUE)

struct _GstM3U8
{
    gchar *uri;                     /* actually downloaded URI */
    gchar *base_uri;                /* URI to use as base for resolving relative
                                       URIs. This will be different to URI in
                                       case of redirects */

    gchar *name;                    /* This will be the "name" of the playlist,
                                       the original relative/absolute URI in a
                                       variant playlist */

    gboolean endlist;               /* if ENDLIST has been reached */
    gint version;                   /* last EXT-X-VERSION */
    GstClockTime targetduration;    /* last EXT-X-TARGETDURATION */
    gboolean allowcache;            /* last EXT-X-ALLOWCACHE */

    gint bandwidth;
    gint program_id;
    gchar *codecs;
    gint width;
    gint height;
    gboolean iframe;
    GList *files;

    /*< private >*/
    gchar *last_data;
    GList *lists;                   /* list of GstM3U8 from the main playlist */
    GList *iframe_lists;            /* I-frame lists from the main playlist */
    GList *current_variant;         /* Current variant playlist used */
    GstM3U8 *parent;                /* main playlist (if any) */
    gint64 mediasequence;           /* EXT-X-MEDIA-SEQUENCE & increased with
                                       new media file */
};

struct _GstM3U8MediaFile
{
    gchar *title;
    GstClockTime duration;
    gchar *uri;
    gint64 sequence;                /* The sequence number of this file */
    gboolean discontinuity;         /* This file marks a discontinuity */
    gchar *key;
    guint8 iv[16];
    gint64 offset, size;
};

struct _GstM3U8Client
{
    GstM3U8 *main;
    GstM3U8 *current;
    guint update_failed_count;
    gint64 sequence;                /* The next sequence for this client */
    GstClockTime sequence_position; /* position of this sequence */
    GMutex lock;
};

GstM3U8Client *gst_m3u8_client_new (const gchar * uri, const gchar * base_uri);
void gst_m3u8_client_free (GstM3U8Client * client);
gboolean gst_m3u8_client_update (GstM3U8Client * client, gchar * data);
gboolean gst_m3u8_client_update_variant_playlist (GstM3U8Client * client,
    gchar * data, const gchar * uri, const gchar * base_uri);
void gst_m3u8_client_set_current (GstM3U8Client * client, GstM3U8 * m3u8);
gboolean gst_m3u8_client_get_next_fragment (
    GstM3U8Client * client,
    gboolean * discontinuity, gchar ** uri, GstClockTime * duration,
    GstClockTime * timestamp, gint64 * range_start, gint64 * range_end,
    gchar ** key, guint8 ** iv, gboolean forward);
void gst_m3u8_client_advance_fragment (GstM3U8Client * client, gboolean forward);
GstClockTime gst_m3u8_client_get_duration (GstM3U8Client * client);
GstClockTime gst_m3u8_client_get_target_duration (GstM3U8Client * client);
gchar * gst_m3u8_client_get_uri (GstM3U8Client * client);
gchar * gst_m3u8_client_get_current_uri (GstM3U8Client * client);
gboolean gst_m3u8_client_has_main (GstM3U8Client * client);
gboolean gst_m3u8_client_has_variant_playlist (GstM3U8Client * client);
gboolean gst_m3u8_client_is_live (GstM3U8Client * client);
GList * gst_m3u8_client_get_playlist_for_bitrate (
    GstM3U8Client * client,
    guint bitrate);

guint64 gst_m3u8_client_get_current_fragment_duration (GstM3U8Client * client);

G_END_DECLS

#endif /* __M3U8_H */
