#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/PosixThreadFactory.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include "gen-cpp/HLServer.h"
#include "gen-cpp/hlserver_constants.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace ::apache::thrift::concurrency;

using boost::shared_ptr;

using namespace  ::hlserver;

class HLServerHandler : virtual public HLServerIf {
 public:
  HLServerHandler() {
    // Your initialization goes here
  }

  void segment(JobStatus& _return, const Properties& properties) {
    // Your implementation goes here
    printf("segment\n");
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

  shared_ptr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(8);
  shared_ptr<PosixThreadFactory> threadFactory = shared_ptr<PosixThreadFactory>(new PosixThreadFactory());
  threadManager->threadFactory(threadFactory);
  threadManager->start();

  TThreadPoolServer server(processor, serverTransport, transportFactory, protocolFactory, threadManager);

  printf("Starting hlserver 0.1...\n");
  server.serve();
  printf("See you later! :D");

  return 0;
}

