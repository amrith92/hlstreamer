#include "segmenter.hpp"
#include <iostream>
#include <chrono>

namespace hlserver {

Segmenter::Segmenter(job_queue &jobs) : jobs_(jobs)
{
    std::cout.sync_with_stdio(true);
    thread_ = std::thread{&Segmenter::process, this};
}

Segmenter::~Segmenter()
{
    thread_.join(); // Wait for current process to end before terminating
}

const JobStatus Segmenter::add_job(const Job& job, const Params& params)
{
    JobStatus status;
    status.type = JobStatusType::ACCEPTED;
    status.jobId = job.id;

    std::lock_guard<std::mutex> lock(mutex_);

    jobs_.push(std::make_tuple(status, job, params, false));
    cond_.notify_all();
    return status;
}

const JobStatus Segmenter::add_job(Job&& job, Params&& params)
{
    JobStatus status;
    status.type = JobStatusType::ACCEPTED;
    status.jobId = job.id;

    std::lock_guard<std::mutex> lock(mutex_);

    jobs_.push(std::make_tuple(status, job, params, false));
    cond_.notify_all();
    return status;
}

void Segmenter::process()
{
    while (true) {
        std::unique_lock<std::mutex> slock(mutex_);
        if (jobs_.empty()) {
            cond_.wait(slock);
        }
        auto job = jobs_.pop();
        slock.unlock();

        if (std::get<0>(job).type == JobStatusType::FINISHED) {
            if (std::get<3>(job) == false) {
                slock.lock();
                jobs_.push(job);
                slock.unlock();
            }
            continue;
        }

        std::cout << "Processing job<" << std::get<1>(job).id << ">...\n";
        std::get<0>(job).type = JobStatusType::IN_PROGRESS;

        slock.lock();
        current_job_status_ = std::get<0>(job);
        slock.unlock();

        std::this_thread::sleep_for(std::chrono::seconds(5)); // simulate processing

        std::get<0>(job).type = JobStatusType::FINISHED;

        // Once the job is done, put it back into the queue.
        slock.lock();
        jobs_.push(job);
        std::cout << "Job <" << std::get<0>(job).jobId << "> finished. Putting it back into the queue (" << jobs_.size() << ")\n";
        slock.unlock();
    }
}

const JobStatus Segmenter::get_status(int64_t jobId)
{
    std::lock_guard<std::mutex> slock(mutex_);
    
    for (auto &job : jobs_.raw()) {
        if (std::get<0>(job).jobId == jobId) {
            return std::get<0>(job);
        }
    }

    JobStatus status;
    status.jobId = 0;
    
    // check current job
    if (current_job_status_.jobId == jobId) {
        status = current_job_status_;
    }

    if (status.jobId != 0) {
        return status;
    }

    status.type = JobStatusType::INVALID;
    status.jobId = jobId;
    return status;
}

void Segmenter::remove_job(const int64_t jobId)
{
    std::lock_guard<std::mutex> slock(mutex_);
    
    auto jobs = jobs_.raw();

    for (auto &job : jobs) {
        if (std::get<0>(job).jobId == jobId) {
            std::get<3>(job) = true; // mark dirty
        }
    }
}

} // namespace hlserver
