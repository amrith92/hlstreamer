#ifndef SEGMENTER_HPP
#define SEGMENTER_HPP

#include "types.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>

namespace hlserver {

class Segmenter
{
public:
    explicit Segmenter(job_queue &jobs);
    virtual ~Segmenter();

    Segmenter(const Segmenter&) = delete;
    Segmenter& operator =(const Segmenter&) = delete;

    const JobStatus add_job(const Job& job, const std::string& in_file);
    const JobStatus add_job(Job&& job, std::string&& in_file);

    /// Processes the jobs in the queue
    void process();
    
    const JobStatus get_status(int64_t jobId);

    void remove_job(int64_t jobId);

private:
    job_queue &jobs_;
    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;
    JobStatus current_job_status_;
};

} // namespace hlserver

#endif /* SEGMENTER_HPP */
