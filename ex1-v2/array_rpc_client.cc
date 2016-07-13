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
using numbers::NumberArray;
using numbers::NumberArrayParameter;
using numbers::ArrayOperator;

class ArrayOperatorClient {
 public:
  ArrayOperatorClient(std::shared_ptr<Channel> channel)
      : stub_(ArrayOperator::NewStub(channel)) {}

  // Assambles the client's payload, sends it and presents the response back
  // from the server.
  //double Pow2(const double value) {
  //  // Data we are sending to the server.
  //  Number in;
  //  in.set_value(value);
  //  // Container for the data we expect from the server.
  //  Number out;
  //  // Context for the client. It could be used to convey extra information to
  //  // the server and/or tweak certain RPC behaviors.
  //  ClientContext context;
  //  // The actual RPC.
  //  Status status = stub_->Pow2(&context, in, &out);
  //  // Act upon its status.
  //  if (status.ok()) {
  //    return out.value();
  //  } else {
  //    return 0.0;
  //  }
  //}

  void arrayPow2(double* vector, size_t vecSize) {
    //break rpc call into chunks
    this->chunkify(&ArrayOperatorClient::_arrayPow2, vector, vecSize, 0.0);
  }

  void arrayInc(double* vector, size_t vecSize) {
    //break rpc call into chunks
    this->chunkify(&ArrayOperatorClient::_arrayInc, vector, vecSize, 0.0);
  }

  void arrayMultiplyBy(double* vector, size_t vecSize, double parameter) {
    //break rpc call into chunks
    this->chunkify(&ArrayOperatorClient::_arrayMultiplyBy, vector, vecSize, parameter);
  }


 private:
  /**/
  void chunkify ( void (ArrayOperatorClient::*function)(double*, size_t, double), double* vector, size_t vecSize, double parameter){
    const size_t limit = 524288;
    size_t chunkSent = 0;
    size_t emptySpace = 0;

    if (vecSize <= limit) {
      //vecSize <= limit: OK TO SEND
      (this->*function)(vector, vecSize, parameter);
    } else {
      size_t iterations = (vecSize/limit);
      // vecSize > limit: BREAK IN iterations+1 chunks
      //full chunks
      for (int i = 0; i < iterations; ++i)
      {
        chunkSent = i*limit;
        (this->*function)(vector+chunkSent, limit, parameter);
      }
      //last chunk
      (this->*function)(vector+((iterations)*limit), vecSize-(iterations*limit), parameter);
    }
  }
  /**/
  // Assambles the client's payload, sends it and presents the response back
  // from the server.
  void _arrayPow2(double* vector, size_t vecSize, double garbage) {

    // Data we are sending to the server.
    NumberArray in;

    for (int i = 0; i < vecSize; ++i)
    {
      in.add_value(vector[i]);
    }

    // Container for the data we expect from the server.
    NumberArray out;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->ArrayPow2(&context, in, &out);

    for (int i = 0; i < vecSize; ++i)
    {
      vector[i] = out.value(i);
    }

    // Act upon its status.
    if (status.ok()) {
      return;
    } else {
      return;
    }
  }

  void _arrayInc(double* vector, size_t vecSize, double garbage) {

    // Data we are sending to the server.
    NumberArray in;

    for (int i = 0; i < vecSize; ++i)
    {
      in.add_value(vector[i]);
    }

    // Container for the data we expect from the server.
    NumberArray out;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->ArrayInc(&context, in, &out);

    for (int i = 0; i < vecSize; ++i)
    {
      vector[i] = out.value(i);
    }

    // Act upon its status.
    if (status.ok()) {
      return;
    } else {
      return;
    }
  }

  void _arrayMultiplyBy(double* vector, size_t vecSize, double parameter) {

    // Data we are sending to the server.
    NumberArrayParameter in;

    in.set_parameter(parameter);
    for (int i = 0; i < vecSize; ++i)
    {
      in.add_value(vector[i]);
    }

    // Container for the data we expect from the server.
    NumberArray out;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->ArrayMultiplyBy(&context, in, &out);

    for (int i = 0; i < vecSize; ++i)
    {
      vector[i] = out.value(i);
    }

    // Act upon its status.
    if (status.ok()) {
      return;
    } else {
      return;
    }
  }

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

double runApplication (int& NUM_THREADS, double* vector, 
                     long& n, size_t& nPerThreads, 
                     int& opcode, double& parameter) {
  std::thread* clients = new std::thread[NUM_THREADS];
  int vectorThreads = 8;
  randomAllocatedVector(vector, n, vectorThreads);

  #ifdef DEBUG
    for (int i = 0; i < n; ++i)
    {
      std::cout << vector[i] << std::endl;
    }
  #endif

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
  double seconds = ((end.tv_sec  - start.tv_sec) * 1000000u + 
    end.tv_usec - start.tv_usec) / 1.e6;

  #ifdef DEBUG
    for (int i = 0; i < n; ++i)
    {
      std::cout << vector[i] << std::endl;
    }
  #endif

  delete [] clients;
  std::cout<<"."<<std::flush;

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

  if (!isdigit(argv[1][0]) || !isdigit(argv[2][0]) || !isdigit(argv[3][0]) || !isdigit(argv[5][0]))
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

  int repeat = 1;
  if (argc > 5) {
    if (isdigit(argv[5][0])){
      repeat = atoi(argv[5]);
    }
  }

  size_t nPerThreads = n/NUM_THREADS;

  double* vector = new double[n];

  std::cout<<"n = "<<n<<" and "<<NUM_THREADS<<" threads"<<std::endl;

  double* times = new double[repeat];

  double meanTime = 0.0;
  for (int i = 0; i < repeat; ++i)
  {
    times[i] = runApplication(NUM_THREADS, vector, n, 
                              nPerThreads, opcode, parameter);
    meanTime += times[i]/(double)repeat;
  }

  double stdDev = 0.0;

  for (int i = 0; i < repeat; ++i)
  {
    stdDev = ((meanTime-times[i])*(meanTime-times[i]))/(double)repeat;
  }
  stdDev = std::sqrt(stdDev);

  std::cout<<"\nMean time: "<<meanTime<<" seconds"<<std::endl;
  std::cout<<"\nStandard Deviation: "<<stdDev<<" seconds"<<std::endl;

  char fname[80];
  FILE* fout;
  sprintf(fname,"times.log");
  fout = fopen(fname,"a");
  fprintf(fout, "vector size: %ld", n);
  fprintf(fout, " nThreads: %d opcode: %d", NUM_THREADS, opcode);
  fprintf(fout, " sdev: %f s \n", stdDev);
  fprintf(fout, " mean time: %f s \n", meanTime);
  fclose(fout);

  delete [] vector;
  /**/
  return 0;
}
