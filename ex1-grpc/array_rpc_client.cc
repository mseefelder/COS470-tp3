#include <thread>
#include <iostream>
#include <memory>
#include <string>
#include <cstring>

#include <grpc++/grpc++.h>

#include "numbers.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::ClientReaderWriter;
using numbers::Number;
using numbers::ArrayOperator;

class ArrayOperatorClient {
 public:
  ArrayOperatorClient(std::shared_ptr<Channel> channel)
      : stub_(ArrayOperator::NewStub(channel)) {}

  // Assambles the client's payload, sends it and presents the response back
  // from the server.
  double Pow2(const double value) {
    // Data we are sending to the server.
    Number in;
    in.set_value(value);

    // Container for the data we expect from the server.
    Number out;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->Pow2(&context, in, &out);

    // Act upon its status.
    if (status.ok()) {
      return out.value();
    } else {
      return 0.0;
    }
  }

  void arrayPow2(double* vector, size_t vecSize) {
   ClientContext context;

    std::shared_ptr<ClientReaderWriter<Number, Number> > stream(
        stub_->ArrayPow2(&context));

    std::thread writer([vector, vecSize, stream]() {
      Number in;
      for (size_t i = 0; i < vecSize; i++) {
        in.set_value(vector[i]);
        stream->Write(in);
      }
      stream->WritesDone();
    });

    Number server_number;
    size_t counter = 0;
    while (stream->Read(&server_number)) {
      vector[counter] = server_number.value();
      counter++;
    }
    writer.join();
    Status status = stream->Finish();
    if (!status.ok()) {
      std::cout << "RouteChat rpc failed." << std::endl;
    }     
  }

 private:
  std::unique_ptr<ArrayOperator::Stub> stub_;
};

int main(int argc, char** argv) {  
  /**/
  int num_clients = 128;
  std::thread* clients = new std::thread[num_clients];
  double* vector = new double[num_clients*2];

  for (int i = 0; i < num_clients*2; ++i)
  {
    vector[i] = (double) i;
  }

  for (int i = 0; i < num_clients; ++i)
  {
    clients[i] = std::thread([i, vector](){
      ArrayOperatorClient greeter(grpc::CreateChannel(
      "localhost:50051", grpc::InsecureChannelCredentials()));
      //double input = (double) i;
      //input = greeter.Pow2(input);
      //std::cout << "Received: " << input << std::endl;
      double *localVector = vector+2*i;
      greeter.arrayPow2(localVector, 2);
    });
  }

  for (int i = 0; i < num_clients; ++i)
  {
    clients[i].join();
  }
  std::cout<<"Threads over"<<std::endl;
  for (int i = 0; i < num_clients*2; ++i)
  {
    std::cout<<vector[i]<<"\n";
  }
  std::cout<<std::endl;
  delete [] vector;
  /**/
  return 0;
}