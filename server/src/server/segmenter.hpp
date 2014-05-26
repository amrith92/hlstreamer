#ifndef SEGMENTER_HPP
#define SEGMENTER_HPP

#include "types.hpp"
#include <thread>

namespace hlserver {

class Segmenter
{
public:
    Segmenter();
    virtual ~Segmenter();

    Segmenter(const Segmenter&) = delete;
    Segmenter& operator =(const Segmenter&) = delete;

    const JobStatus add_job(const Job& job, const Params& params);
    const JobStatus add_job(Job&& job, Params&& params);

    /// Processes the jobs in the queue
    void process();
    
    const JobStatus get_status(int64_t jobId);

private:
    job_queue jobs_;
    std::thread thread_;
};

} // namespace hlserver

#endif /* SEGMENTER_HPP */
