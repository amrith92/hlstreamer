#include "segmenter.h"
#include <gst/gst.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BUFSIZE 128

void grab_videoscale_caps(char *buffer, const uint16_t width, const uint16_t height)
{
	// Sanity checks
	if (buffer == NULL)
		return;

	sprintf(buffer, VSCALE, width, height);
}

void grab_videorate_caps(char *buffer, const uint8_t rate)
{
	// Sanity checks
	if (buffer == NULL)
		return;

	sprintf(buffer, VRATE, rate);
}

void grab_audioresample_caps(char *buffer, const uint16_t rate)
{
	// Sanity checks
	if (buffer == NULL)
		return;

	sprintf(buffer, ARES, rate);
}

const int validate_params(const Params *params)
{
	return 0;
}

int segmenter_try(Params *params)
{
	TranscoderData data;
	GstBus *bus;
	GstMessage *msg;
	GstStateChangeReturn ret;
	gboolean terminate = FALSE;
	char *buffer; // caps buffer
	int retval = 0;

	/// Initialize buffer
	buffer = (char *) malloc(BUFSIZE * sizeof(char));

	if (buffer == NULL) {
		fprintf(stderr, "[%s %s] Could not allocate necessary buffers!", HLS_NAME, HLS_VER);
		return (-1 * BUFSIZE);
	}

	/// Initialize gstreamer
	gst_init(NULL, NULL);

	/// Create elements
	data.filesrc = gst_element_factory_make("filesrc", "filesrc");
	data.source = gst_element_factory_make("decodebin2", "source");
	data.mpegtsmux = gst_element_factory_make("mpegtsmux", "muxer");
	data.progressreport = gst_element_factory_make("progressreport", "[" HLS_NAME " " HLS_VER "]");
	data.videobag.ffmpegcolourspace = gst_element_factory_make("videoconvert", "ffmpegcolourspace");
	data.videobag.videoscale = gst_element_factory_make("videoscale", "videoscale");
	grab_videoscale_caps(buffer, params->video_props.width, params->video_props.height);
	data.videobag.vscalecaps = gst_caps_from_string(buffer);
	data.videobag.videorate = gst_element_factory_make("videorate", "videorate");
	memset(buffer, 0, BUFSIZE);
	grab_videorate_caps(buffer, params->video_props.framerate);
	data.videobag.vratecaps = gst_caps_from_string(buffer);
	data.videobag.aspectratiocrop = gst_element_factory_make("aspectratiocrop", "aspectratiocrop");
	data.videobag.x264enc = gst_element_factory_make("x264enc", "x264enc");
	data.videobag.muxqueue = gst_element_factory_make("queue", "videomuxqueue");
	data.audiobag.decqueue = gst_element_factory_make("queue", "decqueue");
	data.audiobag.audioconvert = gst_element_factory_make("audioconvert", "audioconvert");
	data.audiobag.aconvcaps = gst_caps_from_string(ACONV);
	data.audiobag.audioresample = gst_element_factory_make("audioresample", "audioresample");
	memset(buffer, 0, BUFSIZE);
	grab_audioresample_caps(buffer, params->audio_props.bitrate); /* 22050 */
	data.audiobag.arescaps = gst_caps_from_string(buffer);
	data.audiobag.ffenc_aac = gst_element_factory_make("ffenc_aac", "ffenc_aac");
	data.audiobag.muxqueue = gst_element_factory_make("queue", "audiomuxqueue");
	data.sink = gst_element_factory_make("multifilesink", "sink");

	/// Create empty pipeline
	data.pipeline = gst_pipeline_new("hlsegmenter-pipeline");

	if (!data.pipeline || !data.filesrc || !data.source || !data.videobag.ffmpegcolourspace || !data.videobag.videoscale || !data.videobag.vscalecaps
		|| !data.mpegtsmux || !data.progressreport || !data.sink || !data.videobag.videorate || !data.videobag.vratecaps || !data.videobag.aspectratiocrop
		|| !data.videobag.x264enc || !data.videobag.muxqueue || !data.audiobag.decqueue || !data.audiobag.audioconvert || !data.audiobag.aconvcaps
		|| !data.audiobag.audioresample || !data.audiobag.arescaps || !data.audiobag.ffenc_aac || !data.audiobag.muxqueue) {
		g_printerr("[%s %s] Not all elements could be created!\n", HLS_NAME, HLS_VER);
		return 100;
	}

	/// Set properties
	/// Set the location to transcode & segment
	g_object_set(G_OBJECT(data.filesrc), "location", params->in, NULL);

	/// Set the multifilesink properties
	g_object_set(G_OBJECT(data.sink), "post-messages", TRUE, NULL);
	g_object_set(G_OBJECT(data.sink), "next-file", 2, NULL);
	g_object_set(G_OBJECT(data.sink), "location", params->out, NULL);

	/// Set the update-frequency on progressreport
	g_object_set(G_OBJECT(data.progressreport), "update-freq", 5, NULL);

	/// Set the videorate properties
	g_object_set(G_OBJECT(data.videobag.videorate), "max-rate", params->video_props.framerate, NULL);

	/// Set the aspectratiocrop properties
	g_object_set(G_OBJECT(data.videobag.aspectratiocrop), "aspect-ratio", 16, 9, NULL);

	/// Set x264enc properties
	g_object_set(G_OBJECT(data.videobag.x264enc), "pass", 5, NULL);
	g_object_set(G_OBJECT(data.videobag.x264enc), "quantizer", 24, NULL);
	g_object_set(G_OBJECT(data.videobag.x264enc), "tune", 0x00000004, NULL);
	g_object_set(G_OBJECT(data.videobag.x264enc), "key-int-max", params->video_props.keyinterval, NULL);
	g_object_set(G_OBJECT(data.videobag.x264enc), "byte-stream", TRUE, NULL);
	g_object_set(G_OBJECT(data.videobag.x264enc), "bitrate", params->video_props.bitrate, NULL);
	g_object_set(G_OBJECT(data.videobag.x264enc), "dct8x8", TRUE, NULL);
	g_object_set(G_OBJECT(data.videobag.x264enc), "noise-reduction", params->video_props.noisereduction, NULL);
	g_object_set(G_OBJECT(data.videobag.x264enc), "psy-tune", 1, NULL);

	/// Set ffenc_aac properties
	g_object_set(G_OBJECT(data.audiobag.ffenc_aac), "bitrate", 40960, NULL);

	/// Build the pipeline.
	gst_bin_add_many(GST_BIN(data.pipeline), data.filesrc, data.source, data.videobag.ffmpegcolourspace, data.videobag.videoscale, data.videobag.videorate, data.videobag.aspectratiocrop, data.videobag.x264enc, data.videobag.muxqueue, data.audiobag.decqueue, data.audiobag.audioconvert, data.audiobag.audioresample, data.audiobag.ffenc_aac, data.audiobag.muxqueue, data.mpegtsmux, data.progressreport, data.sink, NULL);

	if (!gst_element_link(data.filesrc, data.source)) {
		g_printerr("[%s %s] File-source could not be linked to demuxer!\n", HLS_NAME, HLS_VER);
		gst_object_unref(data.pipeline);
		return 200;
	}

	if (!gst_element_link(data.videobag.ffmpegcolourspace, data.videobag.videoscale) || !gst_element_link_filtered(data.videobag.videoscale, data.videobag.videorate, data.videobag.vscalecaps) || !gst_element_link_filtered(data.videobag.videorate, data.videobag.aspectratiocrop, data.videobag.vratecaps) || !gst_element_link_many(data.videobag.aspectratiocrop, data.videobag.x264enc, data.videobag.muxqueue, data.mpegtsmux, NULL)) {
		g_printerr("[%s %s] VideoBag elements could not be linked!\n", HLS_NAME, HLS_VER);
		gst_caps_unref(data.videobag.vscalecaps);
		gst_caps_unref(data.videobag.vratecaps);
		gst_object_unref(data.pipeline);
		return 201;
	}
	gst_caps_unref(data.videobag.vscalecaps);
	gst_caps_unref(data.videobag.vratecaps);

	if (!gst_element_link(data.audiobag.decqueue, data.audiobag.audioconvert) || !gst_element_link_filtered(data.audiobag.audioconvert, data.audiobag.audioresample, data.audiobag.aconvcaps) || !gst_element_link_filtered(data.audiobag.audioresample, data.audiobag.ffenc_aac, data.audiobag.arescaps) || !gst_element_link_many(data.audiobag.ffenc_aac, data.audiobag.muxqueue, data.mpegtsmux, NULL)) {
		g_printerr("[%s %s] AudioBag elements could not be linked!\n", HLS_NAME, HLS_VER);
		gst_caps_unref(data.audiobag.aconvcaps);
		gst_caps_unref(data.audiobag.arescaps);
		gst_object_unref(data.pipeline);
		return 202;
	}
	gst_caps_unref(data.audiobag.aconvcaps);
	gst_caps_unref(data.audiobag.arescaps);

	if (!gst_element_link(data.mpegtsmux, data.progressreport)) {
		g_printerr("[%s %s] Muxer could not be linked!\n", HLS_NAME, HLS_VER);
		return 203;
	}

	if (!gst_element_link(data.progressreport, data.sink)) {
		g_printerr("[%s %s] Progress Report could not be linked to sink!\n", HLS_NAME, HLS_VER);
		return 204;
	}

	/// Connect to the pad-added signal
	g_signal_connect(data.source, "pad-added", G_CALLBACK(pad_added_handler), &data);

	/// Start transcoding
	ret = gst_element_set_state(data.pipeline, GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		g_printerr("[%s %s] Unable to set the pipeline to the playing state.\n", HLS_NAME, HLS_VER);
		gst_object_unref(data.pipeline);
		return 102;
	}

	/// Listen to the bus
	bus = gst_element_get_bus(data.pipeline);
	do {
		msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

		/// Parse message
		if (msg != NULL) {
			GError *err;
			gchar *debug_info;

			switch (GST_MESSAGE_TYPE(msg)) {
			case GST_MESSAGE_ERROR:
				gst_message_parse_error(msg, &err, &debug_info);
				g_printerr("[%s %s] Error received from element %s: %s\n", HLS_NAME, HLS_VER, GST_OBJECT_NAME(msg->src), err->message);
				g_printerr("[%s %s] Debugging information: %s\n", HLS_NAME, HLS_VER, debug_info ? debug_info : "none");
				g_clear_error(&err);
				g_free(debug_info);
				terminate = TRUE;
				retval = -1;
				break;

			case GST_MESSAGE_EOS:
				g_print("[%s %s] End-Of-Stream reached!\n", HLS_NAME, HLS_VER);
				terminate = TRUE;
				break;

			case GST_MESSAGE_STATE_CHANGED:
				if (GST_MESSAGE_SRC(msg) == GST_OBJECT(data.pipeline)) {
					GstState old_state, new_state, pending_state;
					gst_message_parse_state_changed(msg, &old_state, &new_state, &pending_state);
					g_print("[%s %s] Pipeline state changed from %s to %s:\n", HLS_NAME, HLS_VER, gst_element_state_get_name(old_state), gst_element_state_get_name(new_state));
				}
				break;

			default:
				g_printerr("[%s %s] Something went wrong!\n", HLS_NAME, HLS_VER);
				break;
			}
		}
	} while (!terminate);

	/// Free resources
	gst_object_unref(bus);
	gst_element_set_state(data.pipeline, GST_STATE_NULL);
	gst_object_unref(data.pipeline);

	return retval;
}

static void pad_added_handler(GstElement *src, GstPad *new_pad, TranscoderData *data)
{
	GstPad *bag = NULL;
	GstPadLinkReturn ret;
	GstCaps *new_pad_caps = NULL;
	GstStructure *new_pad_struct = NULL;
	const gchar *new_pad_type = NULL;

	g_print("[%s %s] Recieved new pad '%s' from '%s':\n", HLS_NAME, HLS_VER, GST_PAD_NAME(new_pad), GST_ELEMENT_NAME(src));

	/// Check new pad's type
	new_pad_caps = gst_pad_get_current_caps(new_pad);
	new_pad_struct = gst_caps_get_structure(new_pad_caps, 0);
	new_pad_type = gst_structure_get_name(new_pad_struct);

	g_print("[%s %s] Pad type is: %s\n", HLS_NAME, HLS_VER, new_pad_type);

	if (g_str_has_prefix(new_pad_type, "video")) {
		g_print("[%s %s] Attempting to retrieve VideoBag\n", HLS_NAME, HLS_VER);
		bag = gst_element_get_static_pad(data->videobag.ffmpegcolourspace, "sink");
	} else if (g_str_has_prefix(new_pad_type, "audio")) {
		g_print("[%s %s] Attempting to retrieve AudioBag\n", HLS_NAME, HLS_VER);
		bag = gst_element_get_static_pad(data->audiobag.decqueue, "sink");
	}

	g_print("[%s %s] Bag status: %d\n", HLS_NAME, HLS_VER, (bag != NULL));

	if (!bag) {
		g_print("[%s %s] Element does not belong to any of the bags!\n", HLS_NAME, HLS_VER);
		goto exit;
	}

	/// Attempt the link
	ret = gst_pad_link(new_pad, bag);
	if (GST_PAD_LINK_FAILED(ret)) {
		g_print("[%s %s] Type is '%s' but link failed.\n", HLS_NAME, HLS_VER, new_pad_type);
	} else {
		g_print("[%s %s] Link succeeded (type '%s').\n", HLS_NAME, HLS_VER, new_pad_type);
	}

exit:
	/// Unreference the new pad's caps
	if (new_pad_caps != NULL)
		gst_caps_unref(new_pad_caps);

	/// Unreference the bag pad
	if (bag != NULL)
		gst_object_unref(bag);
}

#ifdef __cplusplus
}
#endif
