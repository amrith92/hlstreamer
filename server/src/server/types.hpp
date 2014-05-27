#ifndef TYPES_HPP
#define TYPES_HPP

#include <tuple>
#include <segmenter.h>
#include <hlserver_types.h>
#include "concurrent_queue.hpp"

namespace hlserver {
    typedef bool DeleteFlag;
    constexpr bool DELETE_JOB = true;
    constexpr bool KEEP_JOB = false;
    // The last parameter decides if the job should be removed
    // from the queue
    typedef std::tuple<JobStatus, Job, Params, DeleteFlag> QueuedJob;
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
