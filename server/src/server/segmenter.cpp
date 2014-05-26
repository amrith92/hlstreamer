#include "segmenter.hpp"
#include <iostream>

namespace hlserver {

Segmenter::Segmenter()
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

    jobs_.push(std::make_tuple(status, job, params));
    return status;
}

const JobStatus Segmenter::add_job(Job&& job, Params&& params)
{
    JobStatus status;
    status.type = JobStatusType::ACCEPTED;
    status.jobId = job.id;

    jobs_.push(std::make_tuple(status, job, params));
    return status;
}

void Segmenter::process()
{
    while (true) {
        auto job = jobs_.pop();

        std::cout << "Processing job<" << std::get<1>(job).id << ">...\n";
    }
}

const JobStatus Segmenter::get_status(int64_t jobId)
{
    for (auto &job : jobs_.raw()) {
        if (std::get<0>(job).jobId == jobId) {
            return std::get<0>(job);
        }
    }

    JobStatus status;
    status.type = JobStatusType::INVALID;
    status.jobId = jobId;
    return status;
}

} // namespace hlserver
