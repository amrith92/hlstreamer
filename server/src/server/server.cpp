#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/PosixThreadFactory.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <HLServer.h>
#include <hlserver_constants.h>

#include <sstream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "segmenter.hpp"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace ::apache::thrift::concurrency;

using boost::shared_ptr;

using namespace  ::hlserver;

class HLServerHandler : virtual public HLServerIf
{
    Segmenter &segmenter_;
public:
    HLServerHandler(Segmenter &segmenter)
        : segmenter_(segmenter)
    {
    }

    virtual ~HLServerHandler()
    {
        // For later
    }

    void segment(JobStatus& _return, const Properties& properties)
    {
        boost::uuids::uuid id = boost::uuids::random_generator()();
        std::ostringstream oss;
        oss << g_hlserver_constants.BASE_PATH << "/" << id << "/%d.ts";
        Params p = {
            properties.inFile.c_str(),
            oss.str().c_str(),
            {
                static_cast<uint16_t>(properties.video.width),
                static_cast<uint16_t>(properties.video.height),
                static_cast<uint8_t>(properties.video.keyInterval),
                static_cast<uint16_t>(properties.video.bitrate),
                static_cast<uint16_t>(properties.video.framerate),
                static_cast<uint8_t>(properties.video.noiseReduction)
            },
            {
                static_cast<uint16_t>(properties.audio.bitrate),
                static_cast<uint8_t>(properties.audio.channels)
            }
        };

        // Preferrably validate the properties here.
        // Then create a job and add it to the job-queue
        Job job;
        job.id = boost::uuids::hash_value(id) >> 1; // This right shift is required to downsize 128-bit UUIDs to 64-bit job-ids
        job.type = JobType::ON_DEMAND;

        _return = segmenter_.add_job(job, p);
    }

    void status(JobStatus& _return, const int64_t jobId) {
        _return = segmenter_.get_status(jobId);
    }

    void remove(const int64_t jobId) {
        segmenter_.remove_job(jobId);
    }
};

int main(int argc, char **argv)
{
    int port = 0;

    if (argc == 2) {
        port = std::stoi(argv[1]);
    } else {
        port = g_hlserver_constants.SERVER_PORT;
    }

    job_queue jobs;
    Segmenter segmenter(jobs);

    shared_ptr<HLServerHandler> handler(new HLServerHandler(segmenter));
    shared_ptr<TProcessor> processor(new HLServerProcessor(handler));
    shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
    shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
    shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

    shared_ptr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(2);
    shared_ptr<PosixThreadFactory> threadFactory = shared_ptr<PosixThreadFactory>(new PosixThreadFactory());
    threadManager->threadFactory(threadFactory);
    threadManager->start();

    TThreadPoolServer server(processor, serverTransport, transportFactory, protocolFactory, threadManager);

    printf("Starting hlserver 0.1...\n");
    server.serve();
    printf("See you later! :D");

    return 0;
}

