#ifndef TYPES_HPP
#define TYPES_HPP

#include <tuple>
#include <array>
#include <segmenter.h>
#include <hlserver_types.h>
#include "concurrent_queue.hpp"

namespace hlserver {
    /// The Gear type refers to a set of parameters
    /// which control how the media file/stream is
    /// encoded
    typedef Params Gear;

    /// Describes the different video encoding "gears" as
    /// elucidated here: https://developer.apple.com/streaming/#examples
    const VideoParams basic_video_gears[] = {
        /// Gear 1
        { 400 /* width */, 300 /* height */, 100 /* key-int-max */, 232 /* bitrate */, 30 /* framerate */, 150 /* noise-reduction factor */ },

        /// Gear 2
        { 640 /* width */, 480 /* height */, 100 /* key-int-max */, 650 /* bitrate */, 30 /* framerate */, 150 /* noise-reduction factor */ },

        /// Gear 3
        { 640 /* width */, 480 /* height */, 100 /* key-int-max */, 1024 /* bitrate */, 30 /* framerate */, 150 /* noise-reduction factor */ },

        /// Gear 4
        { 960 /* width */, 720 /* height */, 100 /* key-int-max */, 2048 /* bitrate */, 30 /* framerate */, 150 /* noise-reduction factor */ },
    };

    const AudioParams basic_audio_params = { 22050 /* bitrate */, 2 /* channels */ };

    typedef bool DeleteFlag;
    constexpr bool DELETE_JOB = true;
    constexpr bool KEEP_JOB = false;
    // The last parameter decides if the job should be removed
    // from the queue
    typedef std::tuple<JobStatus, Job, const std::string, DeleteFlag> QueuedJob;
    typedef concurrent_queue<QueuedJob> job_queue;

    const std::map<JobStatusType::type, std::string> job_status_type_strings {
        {JobStatusType::INVALID, "INVALID"},
        {JobStatusType::ACCEPTED, "ACCEPTED"},
        {JobStatusType::IN_PROGRESS, "IN_PROGRESS"},
        {JobStatusType::FINISHED, "FINISHED"},
        {JobStatusType::ERROR, "ERROR"}
    };
}

#endif /* TYPES_HPP */
