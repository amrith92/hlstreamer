#include "segmenter.hpp"
#include <hlserver_constants.h>
#include <boost/filesystem.hpp>
#include <cstring>
#include <algorithm>
#include <sstream>
#include <map>
#include <future>
#include <chrono>

#include "playlist.hpp"

using namespace boost::filesystem;

namespace hlserver {

Segmenter::Segmenter(job_queue &jobs) : jobs_(jobs)
{
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

    std::unique_lock<std::mutex> lock(mutex_);

    jobs_.push_back(std::make_tuple(status, job, in_file, KEEP_JOB));
    lock.unlock();
    cond_.notify_one();
    return status;
}

const JobStatus Segmenter::add_job(Job&& job, std::string&& in_file)
{
    JobStatus status;
    status.type = JobStatusType::ACCEPTED;
    status.jobId = job.id;

    std::unique_lock<std::mutex> lock(mutex_);

    jobs_.push_back(std::make_tuple(status, job, in_file, KEEP_JOB));
    lock.unlock();
    cond_.notify_one();
    return status;
}

void Segmenter::process()
{
    while (true) {
        std::unique_lock<std::mutex> slock(mutex_);

        if (jobs_.empty()) {
            cond_.wait(slock);
        }

        auto job = jobs_.front();
        jobs_.pop_front();
        slock.unlock();

        if (std::get<0>(job).type == JobStatusType::FINISHED) {
            if (std::get<3>(job) == KEEP_JOB) {
                slock.lock();
                jobs_.push_back(job);
                slock.unlock();
                cond_.notify_one();
            }
            continue;
        }

        std::get<0>(job).type = JobStatusType::IN_PROGRESS;

        slock.lock();
        current_job_status_ = std::get<0>(job);
        slock.unlock();

        // Build the "gears"
        std::ostringstream g1;
        g1 << g_hlserver_constants.BASE_PATH << "/" << std::get<0>(job).jobId << "/gear1/%d.ts";
        char cg1[60] = {0}, cg2[60] = {0}, cg3[60] = {0}, cg4[60] = {0};
        strncpy(cg1, g1.str().c_str(), 60);
        Gear gear1 {
            std::get<2>(job).c_str(),
            cg1,
            basic_video_gears[0],
            basic_audio_params
        };

        std::ostringstream g2;
        g2 << g_hlserver_constants.BASE_PATH << "/" << std::get<0>(job).jobId << "/gear2/%d.ts";
        strncpy(cg2, g2.str().c_str(), 60);
        Gear gear2 {
            std::get<2>(job).c_str(),
            cg2,
            basic_video_gears[1],
            basic_audio_params
        };

        std::ostringstream g3;
        g3 << g_hlserver_constants.BASE_PATH << "/" << std::get<0>(job).jobId << "/gear3/%d.ts";
        strncpy(cg3, g3.str().c_str(), 60);
        Gear gear3 {
            std::get<2>(job).c_str(),
            cg3,
            basic_video_gears[2],
            basic_audio_params
        };

        std::ostringstream g4;
        g4 << g_hlserver_constants.BASE_PATH << "/" << std::get<0>(job).jobId << "/gear4/%d.ts";
        strncpy(cg4, g4.str().c_str(), 60);
        Gear gear4 {
            std::get<2>(job).c_str(),
            cg4,
            basic_video_gears[3],
            basic_audio_params
        };

        // segmenter_try utilizes all cores whilst running, so
        // I'd prefer calling it serially for each "gear" here
        // The architecture provided by apple states that encoding
        // is a separate process and I understand why, but the
        // gstreamer pipeline handles encoding too. It's not cheating :P
        // The task below basically runs segmenter_try once for
        // each gear and collects the results of each invocation so as
        // to test for errors, if any
        std::packaged_task<std::map<int, std::string>(Gear *, Gear *, Gear *, Gear *)> segment_task
        ([](Gear *gear1, Gear *gear2, Gear *gear3, Gear *gear4)
            {
                std::map<int, std::string> results;
                std::string g1(gear1->out), g2(gear2->out), g3(gear3->out), g4(gear4->out);
                g1.resize(g1.size() - 5);
                g2.resize(g2.size() - 5);
                g3.resize(g3.size() - 5);
                g4.resize(g4.size() - 5);

                // Create the directories g1, g2, g3, g4
                try
                {
                    create_directories(g1);
                    create_directories(g2);
                    create_directories(g3);
                    create_directories(g4);
                }
                catch (const filesystem_error &ex)
                {
                    return results;
                }

                results.emplace(segmenter_try(gear1), g1);
                results.emplace(segmenter_try(gear2), g2);
                results.emplace(segmenter_try(gear3), g3);
                results.emplace(segmenter_try(gear4), g4);

                return results;
            }
        );

        auto fut = segment_task.get_future();
        segment_task(&gear1, &gear2, &gear3, &gear4);
        auto result = fut.get();

        for (const auto &r : result) {
            if (0 == r.first) {
                Playlist::generate(r.second);
            }
        }

        std::get<0>(job).type = JobStatusType::FINISHED;

        // Once the job is done, put it back into the queue.
        slock.lock();
        jobs_.push_back(job);
        slock.unlock();
        cond_.notify_one();
    }
}

const JobStatus Segmenter::get_status(int64_t jobId)
{
    std::unique_lock<std::mutex> slock(mutex_);

    while (jobs_.empty())
    {
        cond_.wait(slock);
    }
    
    for (auto &job : jobs_) {
        if (std::get<0>(job).jobId == jobId) {
            slock.unlock();
            return std::get<0>(job);
        }
    }

    JobStatus status;
    status.jobId = 0;
    
    // check current job
    if (current_job_status_.jobId == jobId) {
        status = current_job_status_;
    }

    slock.unlock();

    if (status.jobId != 0) {
        return status;
    }

    status.type = JobStatusType::INVALID;
    status.jobId = jobId;
    return status;
}

void Segmenter::remove_job(const int64_t jobId)
{
    std::unique_lock<std::mutex> slock(mutex_);

    while (jobs_.empty())
    {
        cond_.wait(slock);
    }

    auto job = std::find_if(jobs_.begin(), jobs_.end(), [&jobId] (const QueuedJob& j)
        {
            if (std::get<0>(j).jobId == jobId) {
                return true;
            }

            return false;
        }
    );

    while (job == std::end(jobs_)) {
        cond_.wait(slock);
        job = std::find_if(jobs_.begin(), jobs_.end(), [&jobId] (const QueuedJob& j)
            {
                if (std::get<0>(j).jobId == jobId) {
                    return true;
                }

                return false;
            }
        );
    }

    if (job != std::end(jobs_)) {
        std::get<3>(*job) = DELETE_JOB;
    }

    slock.unlock();
}

} // namespace hlserver
