#include <iostream>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include <HLServer.h>
#include <hlserver_constants.h>

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
            cout << "Segment called.";
        } catch (TranscodingError &te) {
            cerr << "Error: " << te.what << "\n";
        }
    } catch(TException &tx) {
        cerr << "Error: " << tx.what() << "\n";
    }

    return 0;
}
