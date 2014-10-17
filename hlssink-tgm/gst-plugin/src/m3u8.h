#ifndef __M3U8_H
#define __M3U8_H

#include <glib.h>

G_BEGIN_DECLS

typedef struct _GstM3U8 GstM3U8;
typedef struct _GstM3U8MediaFile GstM3U8MediaFile;
typedef strut _GstM3U8Client GstM3U8Client;

#define GST_M3U8(m) ((GstM3U8 *) m)
#define GST_M3U8_MEDIA_FILE(f) ((GstM3U8MediaFile *) f)

#define GST_M3U8_CLIENT_LOCK(c) g_mutex_lock (&c->lock);
#define GST_M3U8_CLIENT_UNLOCK(c) g_mutex_unlock (&c->lock);

#define GSST_M3U8_CLIENT_IS_LIVE(c) ((! (c)->current || (c)->current->endlist) ? FALSE : TRUE)

struct _GstM3U8
{
    gchar *uri;

    gboolean endlist;
    gint version;                   /* last EXT-X-VERSION */
    GstClockTime targetduration;    /* last EXT-X-TARGETDURATION */
    gchar *allowcache;              /* last EXT-X-ALLOWCACHE */
    gchar *key;


    gint bandwidth;
    gint program_id;
    gchar *codecs;
    gint width;
    gint height;
    GList *files;

    /*< private >*/
    gchar *last_data;
    GList *lists;
    GList *current_variant;
    GstM3U8 *parent;
    guint mediasequence; /* EXT-X-MEDIA-SEQUENCE & increased with new media file */
};

struct _GstM3U8MediaFile
{
    gchar *title;
    GstClockTime duration;
    gchar *uri;
    guint sequence;
    gboolean discontinuity;
    gchar *key;
    guint8 iv[16];
    gint64 offset, size;
};

struct _GstM3U8Client
{
    GstM3U8 *main;
    GstM3U8 *current;
    guint update_failed_count;
    gint sequence;
    GstClockTime sequence_position;
    GMutex lock;
};

GstM3U8Client *gst_m3u8_client_new (const gchar * uri);
void gst_m3u8_client_free (GstM3U8Client * client);
gboolean gst_m3u8_client_update (GstM3U8Client * client, gchar * data);
void gst_m3u8_client_set_current (GstM3U8Client * client, GstM3U8 * m3u8);
gboolean gst_m3u8_client_get_next_fragment (
    GstM3U8Client * client,
    gboolean * discontinuity, const gchar ** uri, GstClockTime * duration,
    GstClockTime * timestamp, gint64 * range_start, gint64 * range_end,
    const gchar ** key, const guint8 ** iv);
void gst_m3u8_client_advance_fragment (GstM3U8Client * client);
GstClockTime gst_m3u8_client_get_duration (GstM3U8Client * client);
GstClockTime gst_m3u8_client_get_target_duration (GstM3U8Client * client);
const gchar * gst_m3u8_client_get_uri (GstM3U8Client * client);
const gchar * get_m3u8_client_get_current_uri (GstM3U8Client * client);
gboolean gst_m3u8_client_has_variant_playlist (GstM3U8Client * client);
gboolean gst_m3u8_client_is_live (GstM3U8Client * client);
GList * gst_m3u8_client_get_playlist_for_bitrate (
    GstM3U8Client * client,
    guint bitrate);

guint64 gst_m3u8_client_get_current_fragment_duration (GstM3U8Client * client);

G_END_DECLS

#endif /* __M3U8_H */
