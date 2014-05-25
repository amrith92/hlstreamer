#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/PosixThreadFactory.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include "gen-cpp/HLServer.h"
#include "gen-cpp/hlserver_constants.h"

#include <ctime>
#include <sstream>
#include <deque>
#include <segmenter.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace ::apache::thrift::concurrency;

using boost::shared_ptr;

using namespace  ::hlserver;

class HLServerHandler : virtual public HLServerIf
{
    typedef std::pair<Job, Params> QueuedJob;
    std::deque<QueuedJob> jobs_;

public:
    HLServerHandler()
    {
        // Initialize stuff here
    }

    virtual ~HLServerHandler()
    {
        if (jobs_.size() > 0) {
            jobs_.pop_front();
        }
    }

    void segment(JobStatus& _return, const Properties& properties)
    {
        std::ostringstream oss;
        oss << g_hlserver_constants.BASE_PATH << time(NULL) << "/%d.ts";
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
        job.id = time(NULL);
        job.type = JobType::ON_DEMAND;
        this->jobs_.push_back(std::make_pair(job, p));
        std::cout << "There are now " << this->jobs_.size() << " jobs in the queue\n";
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

    shared_ptr<HLServerHandler> handler(new HLServerHandler());
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

