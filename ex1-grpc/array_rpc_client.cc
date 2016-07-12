#include <thread>
#include <iostream>
#include <memory>
#include <string>
#include <cstring>
#include <random>
#include <unistd.h>

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

void fillArrayCorrect(int tid, long n, double* element, int numThreads) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(-100,100);
  int nPerThread = n/numThreads;
  int index = tid * nPerThread;
  for ( int i = index; i < index + nPerThread; i ++) {
    element[i] = dis(gen);
  }
}

void randomAllocatedVector (double* element, long n, int numThreads) {
  std::thread *th = new std::thread[numThreads];

  for( int i = 0; i < numThreads ; i++) {
    th[i] = std::thread(fillArrayCorrect,i,n,element, numThreads);
  }

  for(int i = 0; i < numThreads; i++) {
    th[i].join();
  }
  delete [] th;
}



int main(int argc, char** argv) {
  /**/
  if (argc < 3)
    {
      std::cout<<"Usage is:\n"<<argv[0]<<
        " <amount of numbers to generate>"<<
        "<amount of threads>" <<
        std::endl;
      return 0;
    }

  if (!isdigit(argv[1][0]))
    {
      std::cerr<<"Amount of numbers to generate should be a number!"<<std::endl;
      return 0;
  }

  long n = atol(argv[1]);
  int NUM_THREADS = atoi(argv[2]);

  size_t nPerThreads = n/NUM_THREADS;

  std::thread* clients = new std::thread[NUM_THREADS];
  double* vector = new double[n];
  int vectorThreads = 8;
  randomAllocatedVector(vector, n, vectorThreads);

  //verify elements on vector ---> just test
  for (int i = 0; i < n; ++i)
  {
    std::cout << vector[i] << std::endl;
  }

  for (int i = 0; i < NUM_THREADS; ++i)
  {
    clients[i] = std::thread([i, vector, nPerThreads](){
      ArrayOperatorClient greeter(grpc::CreateChannel(
      "localhost:50051", grpc::InsecureChannelCredentials()));
      //double input = (double) i;
      //input = greeter.Pow2(input);
      //std::cout << "Received: " << input << std::endl;
      double *localVector = vector + nPerThreads*i;
      greeter.arrayPow2(localVector, nPerThreads);
    });
  }

  for (int i = 0; i < NUM_THREADS; ++i)
  {
    clients[i].join();
  }
  std::cout<<"Threads over"<<std::endl;

  for (int i = 0; i < n; ++i)
  {
    std::cout<<vector[i]<<"\n";
  }
  std::cout<<std::endl;

  delete [] vector;
  /**/
  return 0;
}
