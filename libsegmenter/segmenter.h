#ifndef _SEGMENTER_H_DEFINED
#define _SEGMENTER_H_DEFINED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <gst/gst.h>

#define HLS_NAME	"hlsegmenter"
#define HLS_VER		"0.1"

/// Caps strings
#define VSCALE		"video/x-raw,width=%d,height=%d"
#define VRATE		"video/x-raw, framerate=%d/1"
//#define ACONV		"audio/x-raw,channels=2"
#define ARES		"audio/x-raw,rate=%d,channels=2"

typedef struct _video_bag {
	GstElement *videoconvert;
	GstElement *videoscale;
	GstCaps	   *vscalecaps;
	GstElement *videorate;
	GstCaps    *vratecaps;
	GstElement *aspectratiocrop;
	GstElement *x264enc;
	GstElement *muxqueue;
} VideoBag;

typedef struct _audio_bag {
	GstElement *decqueue;
	GstElement *audioconvert;
	GstElement *audiorate;
	GstElement *audioresample;
	GstCaps    *arescaps;
	GstElement *voaacenc;
	GstElement *muxqueue;
} AudioBag;

typedef struct _transcoder_data {
	GstElement *pipeline;
	GstElement *filesrc;
	GstElement *source;
	GstElement *mpegtsmux;
	GstElement *progressreport;
	VideoBag videobag;
	AudioBag audiobag;
	GstElement *sink;
} TranscoderData;

typedef struct _video_params {
	const uint16_t width;
	const uint16_t height;
	const uint8_t keyinterval;
	const uint16_t bitrate;
	const uint16_t framerate;
	const uint8_t noisereduction;
} VideoParams;

typedef struct _audio_params {
	const uint16_t bitrate;
	const uint8_t channels;
} AudioParams;

typedef struct _params {
	const char *in;
	const char *out;
	VideoParams video_props;
	AudioParams audio_props;
} Params;

/**
 *	TODO: Validate incoming parameters
 */
extern const int validate_params(const Params *params);

/**
	 Handler for the pad-added signal
*/
static void pad_added_handler(GstElement *src, GstPad *new_pad, TranscoderData *data);

/**
	The segmenter must be callable from the server, and as such the
	following function is the entry-point
*/
extern int segmenter_try(Params *params);

#ifdef __cplusplus
}
#endif

#endif /* _SEGMENTER_H_DEFINED */
