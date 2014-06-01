#include <iostream>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include <HLServer.h>
#include <hlserver_constants.h>
#include "../server/types.hpp"

#include <thread>
#include <chrono>

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace hlserver;

int main()
{
    boost::shared_ptr<TTransport> socket(new TSocket("localhost", g_hlserver_constants.SERVER_PORT));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    HLServerClient client(protocol);

    try {
        transport->open();

        std::string file = "/home/amrith92/Videos/a.mp4";

        JobStatus status;
        
        try {
            client.segment(status, file, JobType::ON_DEMAND);
            cout << "New job <" << status.jobId << "> submitted.\n";

            client.status(status, status.jobId);
            while (status.type != JobStatusType::FINISHED && status.type != JobStatusType::ERROR) {
                cout << "Job <" << status.jobId << "> status: " << job_status_type_strings.at(status.type) << "\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                client.status(status, status.jobId);
            }
            cout << "Job <" << status.jobId << "> status: " << job_status_type_strings.at(status.type) << "\n";

            client.retire(status.jobId);
            cout << "Retired job.\n";
        } catch (TranscodingError &te) {
            cerr << "Transcoding Error: " << te.what << "\n";
        } catch (JobError &je) {
            cerr << "Job Error: " << je.what << "\n";
        }
    } catch(TException &tx) {
        cerr << "Error: " << tx.what() << "\n";
    }

    return 0;
}
