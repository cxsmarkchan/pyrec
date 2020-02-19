#ifndef PATH_TO_PROTO_HLRPC_TEST_HLRPC_PB_H_
#define PATH_TO_PROTO_HLRPC_TEST_HLRPC_PB_H_

#include <memory>

#include "path/to/proto/hlrpc_test.grpc.pb.h"
#include "core/util/ip.h"
#include "core/util/status.h"

namespace test {

namespace code_gen {

class TestServiceHybridBase : public TestService::Service {
 public:
  int Run(const Address& address) {
    grpc::ServerBuilder builder;
    builder.AddListeningPort(address.ToString(), grpc::InsecureServerCredentials());
    builder.RegisterService(this);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    server->Wait();

    return 0;
  }

  grpc::Status TestRpc(grpc::ServerContext* context, const module::request::Request* request, module::reply::Reply* reply) final {
    pyrec::Status status = TestRpcProcess(request, reply);
    return status.ToGrpcStatus();
  }

  pyrec::Status TestRpcLocal(const module::request::Request* request, module::reply::Reply* reply) {
    return TestRpcProcess(request, reply);
  }

  virtual pyrec::Status TestRpcProcess(const module::request::Request* request, module::reply::Reply* reply) = 0;

  grpc::Status TestRpc2(grpc::ServerContext* context, const module::request::Request2* request, module::reply::Reply2* reply) final {
    pyrec::Status status = TestRpc2Process(request, reply);
    return status.ToGrpcStatus();
  }

  pyrec::Status TestRpc2Local(const module::request::Request2* request, module::reply::Reply2* reply) {
    return TestRpc2Process(request, reply);
  }

  virtual pyrec::Status TestRpc2Process(const module::request::Request2* request, module::reply::Reply2* reply) = 0;
};

}  // namespace code_gen

}  // namespace test

#endif  // PATH_TO_PROTO_HLRPC_TEST_HLRPC_PB_H_
