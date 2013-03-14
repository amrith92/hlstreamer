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
#define VSCALE		"video/x-raw-yuv,width=%d,height=%d"
#define VRATE		"video/x-raw-yuv, framerate=%d/1"
#define ACONV		"audio/x-raw-int,channels=2"
#define ARES		"audio/x-raw-int,rate=%d,channels=2"

typedef struct _video_bag {
	GstElement *ffmpegcolourspace;
	GstElement *videoscale;
	GstCaps	   *vscalecaps;
	GstElement *videorate;
	GstCaps    *vratecaps;
	GstElement *aspectratiocrop;
	GstElement *x264enc;
	GstElement *muxqueue;
} VideoBag;

/**
 *	Grab customized videoscale caps string
 */
extern void grab_videoscale_caps(char *buffer, const uint16_t width, const uint16_t height);

/**
 *	Grab customized videorate caps string
 */
extern void grab_videorate_caps(char *buffer, const uint8_t rate);

typedef struct _audio_bag {
	GstElement *decqueue;
	GstElement *audioconvert;
	GstCaps    *aconvcaps;
	GstElement *audioresample;
	GstCaps    *arescaps;
	GstElement *ffenc_aac;
	GstElement *muxqueue;
} AudioBag;

/**
 *	Grab customized audioresample caps string
 */
extern void grab_audioresample_caps(char *buffer, const uint16_t rate);

typedef struct _transcoder_data {
	GstElement *pipeline;
	GstElement *filesrc;
	GstElement *source;
	GstElement *mpegtsmux;
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
 *	Validate incoming parameters
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
