/**
 *  This describes the hlserver as a Apache Thrift
 *  service descriptor.
 *
 */

namespace cpp hlserver

typedef byte i8

const i16 SERVER_PORT = 9822
const string BASE_PATH = "/srv/hlstreamer"

/**
 *  Represents a job type.
 */
enum JobType {
    ON_DEMAND = 1,      // For video on-demand (non-realtime)
    LIVE_STREAM = 2,    // For real time pipelines
    SCHEDULING = 3      // For edits/scheduled shows (with ads)
}

/**
 *  Represents a Job, which is a transcoding-segmenting
 *  pipeline
 */
struct Job {
    1: i64 id,
    2: JobType type,
}

/**
 *  Encodes job status values
 */
enum JobStatusType {
    INVALID = 1,        // Magical Mystery Job
    ACCEPTED = 2,       // Job has been accepted
    IN_PROGRESS = 3,    // Job is in progress
    FINISHED = 4,       // Job is successfully completed
    ERROR = 6,          // was FUCKED, changed it to this :/
}

/**
 *  Represents the status of the job
 *  in question
 */
struct JobStatus {
    1: JobStatusType type,
    2: i64 jobId,
    3: optional string message,
}

/**
 *  Represents an error raised during
 *  the transcoding process.
 */
exception TranscodingError {
    1: i32 code,
    2: string what
}

/**
 *  Represents an error in a job
 */
exception JobError {
    1: i64 jobId,
    2: string what
}

/**
 *  The main service for HLServer. This is largely incomplete.
 */
service HLServer {
    /// Submit a new file to be segmented
    JobStatus segment(1: string in_file, 2: JobType job_type = JobType.ON_DEMAND) throws (1:TranscodingError te, 2: JobError je),

    /// Check status of job
    JobStatus status(1:i64 jobId) throws (1:JobError je),

    /// Delete a job once the client sends this message
    oneway void retire(1:i64 jobId),
}
