#include "snapper.h"

#include <gst/gst.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include <stdlib.h>

int8_t generate_snaps_of(const char *scan_path, const char *dest_path)
{
	return 0;
}

int8_t generate_snap_of(const char *path, const char *dest_path)
{
	GstElement *pipeline, *sink;
	gint width, height;
	GstSample *sample;
	gchar *description, *filename;
	GError *error = NULL;
	GdkPixbuf *pixbuf;
	gint64 duration, position;
	GstStateChangeReturn ret;
	gboolean res;
	GstMapInfo map;

	if (path == NULL || dest_path == NULL) {
		return -1;
	}

	filename = g_strdup_printf("%s/%s.png", dest_path, path);

	description = g_strdup_printf("uridecodebin uri=%s ! videoconvert ! videoscale ! appsink name=sink caps=\"" _SNAPS_CAPS "\"", path);

	pipeline = gst_parse_launch(description, &error);

	if (error != NULL) {
		g_printerr("Unable to construct pipeline!");
		g_error_free(error);
		return -2;
	}

	sink = gst_bin_get_by_name(GST_BIN(pipeline), "sink");

	ret = gst_element_set_state(pipeline, GST_STATE_PAUSED);
	switch (ret) {
	case GST_STATE_CHANGE_FAILURE:
		return -101;
	break;

	case GST_STATE_CHANGE_NO_PREROLL:
		return -102;
	break;

	default:
		break;
	}

	ret = gst_element_get_state(pipeline, NULL, NULL, 5 * GST_SECOND);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		return -201;
	}

	gst_element_query_duration(pipeline, GST_FORMAT_TIME, &duration);
	if (duration != -1) {
		position = duration * 5 / 100;
	} else {
		position = 1 * GST_SECOND;
	}

	/// Seek to the position in the file
	gst_element_seek_simple(pipeline, GST_FORMAT_TIME, GST_SEEK_FLAG_KEY_UNIT | GST_SEEK_FLAG_FLUSH, position);

	/// get the preroll buffer from appsink
	g_signal_emit_by_name(sink, "pull-preroll", &sample, NULL);

	if (sample) {
		GstBuffer *buffer;
		GstCaps *caps;
		GstStructure *s;

		caps = gst_sample_get_caps(sample);

		if (!caps) {
			g_printerr("Could not retrieve snap!");
			return -301;
		}

		s = gst_caps_get_structure(caps, 0);
		res = gst_structure_get_int(s, "width", &width);
		res |= gst_structure_get_int(s, "height", &height);

		if (!res) {
			g_printerr("Could not figure out snap-dimension!");
			return -302;
		}

		buffer = gst_sample_get_buffer(sample);
		gst_buffer_map(buffer, &map, GST_MAP_READ);
		pixbuf = gdk_pixbuf_new_from_data(map.data, GDK_COLORSPACE_RGB, FALSE, 8, width, height, GST_ROUND_UP_4(width * 3), NULL, NULL);

		// save the pixbuf
		gdk_pixbuf_save (pixbuf, filename, "png", &error, NULL);
		gst_buffer_unmap(buffer, &map);
	} else {
		g_printerr("could not print snapshot!");
	}

	gst_element_set_state (pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);

	return 0;
}
