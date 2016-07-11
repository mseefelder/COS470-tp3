#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>

#include "numbers.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using grpc::ServerReaderWriter;
using numbers::Number;
using numbers::ArrayOperator;

// Logic and data behind the server's behavior.
class ArrayOperatorServiceImpl final : public ArrayOperator::Service {
  Status Pow2(ServerContext* context, const Number* in,
                  Number* out) override {
    out->set_value(in->value()*in->value());
    return Status::OK;
  }

  Status ArrayPow2(ServerContext* context,
                   ServerReaderWriter<Number, Number>* stream) override {
    Number number;
    while (stream->Read(&number)) {
      number.set_value(number.value()*number.value());
      stream->Write(number);
    }

    return Status::OK;
  }
};

void RunServer() {
  std::string server_address("0.0.0.0:50051");
  ArrayOperatorServiceImpl service;

  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

int main(int argc, char** argv) {
  RunServer();

  return 0;
}