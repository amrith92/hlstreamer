#include "segmenter.hpp"
#include <iostream>
#include <sstream>
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

const JobStatus Segmenter::add_job(const Job& job, const std::string& in_file)
{
    JobStatus status;
    status.type = JobStatusType::ACCEPTED;
    status.jobId = job.id;

    std::lock_guard<std::mutex> lock(mutex_);

    jobs_.push(std::make_tuple(status, job, in_file, KEEP_JOB));
    cond_.notify_all();
    return status;
}

const JobStatus Segmenter::add_job(Job&& job, std::string&& in_file)
{
    JobStatus status;
    status.type = JobStatusType::ACCEPTED;
    status.jobId = job.id;

    std::lock_guard<std::mutex> lock(mutex_);

    jobs_.push(std::make_tuple(status, job, in_file, KEEP_JOB));
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
            if (std::get<3>(job) == KEEP_JOB) {
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

        // Build the "gears"
        std::ostringstream g1;
        g1 << g_hlserver_constants.BASE_PATH << "/" << std::get<0>(job).jobId << "-gear1/%d.ts";
        Gear gear1 {
            std::get<2>(job),
            g1.str(),
            basic_video_gears[0],
            basic_audio_params
        };

        std::ostringstream g2;
        g2 << g_hlserver_constants.BASE_PATH << "/" << std::get<0>(job).jobId << "-gear2/%d.ts";
        Gear gear2 {
            std::get<2>(job),
            g2.str(),
            basic_video_gears[1],
            basic_audio_params
        };

        std::ostringstream g3;
        g3 << g_hlserver_constants.BASE_PATH << "/" << std::get<0>(job).jobId << "-gear3/%d.ts";
        Gear gear3 {
            std::get<2>(job),
            g3.str(),
            basic_video_gears[2],
            basic_audio_params
        };

        std::ostringstream g4;
        g4 << g_hlserver_constants.BASE_PATH << "/" << std::get<0>(job).jobId << "-gear4/%d.ts";
        Gear gear4 {
            std::get<2>(job),
            g4.str(),
            basic_video_gears[3],
            basic_audio_params
        };

        // segmenter_try utilizes all cores whilst running, so
        // I'd prefer calling it serially for each "gear" here
        // The architecture provided by apple states that encoding
        // is a separate process and I understand why, but the
        // gstreamer pipeline handles encoding too. It's not cheating :P
        segmenter_try(&gear1);
        segmenter_try(&gear2);
        segmenter_try(&gear3);
        segmenter_try(&gear4);

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
            std::get<3>(job) = DELETE_JOB; // mark dirty
        }
    }
}

} // namespace hlserver
