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

        Properties properties;
        properties.inFile = "";
        properties.baseUri = "http://aesahaettr/";

        JobStatus status;
        
        try {
            client.segment(status, properties);
            cout << "New job <" << status.jobId << "> submitted.\n";

            client.status(status, status.jobId);
            while (status.type != JobStatusType::FINISHED && status.type != JobStatusType::ERROR) {
                cout << "Job <" << status.jobId << "> status: " << job_status_type_strings.at(status.type) << "\n" << flush;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                client.status(status, status.jobId);
            }
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
