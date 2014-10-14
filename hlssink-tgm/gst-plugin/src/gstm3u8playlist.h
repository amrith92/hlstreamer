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
#ifndef __GST_M3U8_PLAYLIST_H
#define __GST_M3U8_PLAYLIST_H

#include <glib.h>
#include <gio/gio.h>

G_BEGIN_DECLS

typedef struct _GstM3U8Playlist GstM3U8Playlist;
typedef struct _GstM3U8Entry GstM3U8Entry;

struct _GstM3U8Entry
{
    gfloat duration;
    gchar *title;
    gchar *url;
    GFile *file;
    gboolean discontinuous;
};

struct _GstM3U8Playlist
{
    guint version;
    gboolean allow_cache;
    gint window_size;
    gint type;
    gboolean end_list;
    guint sequence_number;

    /*< Private >*/
    GQueue *entries;
    GString *playlist_str;
};

GstM3U8Playlist * gst_m3u8_playlist_new (guint version,
                                         guint window_size,
                                         gboolean allow_cache);
void gst_m3u8_playlist_free (GstM3U8Playlist * playlist);
gboolean gst_m3u8_playlist_add_entry (GstM3U8Playlist * playlist,
                                      const gchar * url,
                                      GFile * file,
                                      const gchar *title,
                                      gfloat duration,
                                      guint index,
                                      gboolean discontinuous);
gchar * gst_m3u8_playlist_render (GstM3U8Playlist * playlist);
void gst_m3u8_playlist_clear (GstM3U8Playlist * playlist);
guint gst_m3u8_playlist_n_entries (GstM3U8Playlist * playlist);

G_END_DECLS

#endif /* __GST_M3U8_PLAYLIST_H */
