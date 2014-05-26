#ifndef TYPES_HPP
#define TYPES_HPP

#include <tuple>
#include <segmenter.h>
#include <hlserver_constants.h>
#include "concurrent_queue.hpp"

namespace hlserver {
    typedef std::tuple<JobStatus, Job, Params> QueuedJob;
    typedef concurrent_queue<QueuedJob> job_queue;
}

#endif /* TYPES_HPP */
