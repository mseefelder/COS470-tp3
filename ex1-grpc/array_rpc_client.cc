#include <thread>
#include <iostream>
#include <memory>
#include <string>
#include <cstring>
#include <cstdio>
#include <random>
#include <unistd.h>
#include <sys/time.h>

#include <grpc++/grpc++.h>

#include "numbers.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::ClientReaderWriter;
using numbers::Number;
using numbers::NumberParameter;
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

  void arrayInc(double* vector, size_t vecSize) {
   ClientContext context;

    std::shared_ptr<ClientReaderWriter<Number, Number> > stream(
        stub_->ArrayInc(&context));

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

  void arrayMultiplyBy(double* vector, size_t vecSize, double parameter) {
   ClientContext context;

    std::shared_ptr<ClientReaderWriter<NumberParameter, Number> > stream(
        stub_->ArrayMultiplyBy(&context));

    std::thread writer([vector, vecSize, stream, parameter]() {
      NumberParameter in;
      for (size_t i = 0; i < vecSize; i++) {
        in.set_number(vector[i]);
        in.set_parameter(parameter);
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

float runApplication (int& NUM_THREADS, double* vector, 
                     long& n, size_t& nPerThreads, 
                     int& opcode, double& parameter) {
  std::thread* clients = new std::thread[NUM_THREADS];
  int vectorThreads = 8;
  randomAllocatedVector(vector, n, vectorThreads);

  struct timeval start, end;
  gettimeofday(&start, NULL);

  for (int i = 0; i < NUM_THREADS; ++i)
  {
    clients[i] = std::thread([i, vector, nPerThreads, opcode, parameter](){
      ArrayOperatorClient greeter(grpc::CreateChannel(
      "localhost:50051", grpc::InsecureChannelCredentials()));
      
      double *localVector = vector + nPerThreads*i;
      switch (opcode) {
        case 0:
          greeter.arrayInc(localVector, nPerThreads);
          break;
        case 1:
          greeter.arrayPow2(localVector, nPerThreads);
          break;
        case 2:
          greeter.arrayMultiplyBy(localVector, nPerThreads, parameter);
          break;
        default:
          break;
      }
      
    });
  }

  for (int i = 0; i < NUM_THREADS; ++i)
  {
    clients[i].join();
  }

  gettimeofday(&end, NULL);
  float seconds = ((end.tv_sec  - start.tv_sec) * 1000000u + 
    end.tv_usec - start.tv_usec) / 1.e6;

  delete [] clients;
  std::cout<<".";

  return seconds;
}

int main(int argc, char** argv) {
  /**/
  if (argc < 4)
  {
    std::cout<<"Usage is:\n"<<argv[0]<<
      " <amount of numbers to generate>"<<
      " <amount of threads>" <<
      " <opcode = 0,1,2>" << 
      " <optional: parameter>\n" <<
      " OP CODES:\n" <<
      "    0 - Increment by 1;\n" <<
      "    1 - Apply power of 2;\n" <<
      "    2 - Multiply by parameter;\n" <<
      "parameter default = 4.0" <<
      std::endl;
    return 0;
  }

  if (!isdigit(argv[1][0]) || !isdigit(argv[2][0]) || !isdigit(argv[3][0]))
  {
    std::cerr<<"Arguments should be numbers!"<<std::endl;
    return 0;
  }

  long n = atol(argv[1]);
  int NUM_THREADS = atoi(argv[2]);
  int opcode = atoi(argv[3]);
  if (opcode < 0 || opcode > 2) {
    std::cerr<<"IMPOSSIBLE OPCODE!"<<std::endl;
    return 0;
  }

  double parameter = 4.0;
  if (argc > 4) {
    if (isdigit(argv[4][0])){
      parameter = atof(argv[4]);
    }
  }
  #ifdef DEBUG
    std::cout<<"Parameter is:"<<parameter<<std::endl;      
  #endif

  size_t nPerThreads = n/NUM_THREADS;

  double* vector = new double[n];

  #ifdef DEBUG
    for (int i = 0; i < n; ++i)
    {
      std::cout << vector[i] << std::endl;
    }
  #endif

  std::cout<<"n = "<<n<<" and "<<NUM_THREADS<<" threads"<<std::endl;

  float meanTime = 0.0;
  for (int i = 0; i < 10; ++i)
  {
    meanTime += runApplication(NUM_THREADS, vector, n, 
                              nPerThreads, opcode, parameter)/10.0;
  }

  #ifdef DEBUG
    for (int i = 0; i < n; ++i)
    {
      std::cout<<vector[i]<<"\n";
    }
  #endif
  std::cout<<"\nMean time: "<<meanTime<<" seconds"<<std::endl;

  char fname[80];
  FILE* fout;
  sprintf(fname,"times.log");
  fout = fopen(fname,"a");
  fprintf(fout, "vector size: %ld", n);
  fprintf(fout, " nThreads: %d opcode: %d", NUM_THREADS, opcode);
  fprintf(fout, " mean time: %f s \n", meanTime);
  fclose(fout);

  delete [] vector;
  /**/
  return 0;
}
