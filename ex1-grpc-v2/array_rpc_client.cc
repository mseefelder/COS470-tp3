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

  //Break thread vector into smaller chunks to avoid grpc warnings or errors regarding big messages
  void chunkify ( void (ArrayOperatorClient::*function)(double*, size_t, double), 
                  double* vector, 
                  size_t vecSize, 
                  double parameter)
  {
    //adequate limit for double quantity
    const size_t limit = 524288;
    //amount of doubles already sent
    size_t chunkSent = 0;

    if (vecSize <= limit) {
      //vecSize <= limit: OK TO SEND
      (this->*function)(vector, vecSize, parameter);
    } else {
      size_t iterations = (vecSize/limit);
      // vecSize > limit: BREAK IN iterations+1 chunks
      //full chunks are those which have 'limit' doubles
      for (int i = 0; i < iterations; ++i)
      {
        chunkSent = i*limit;
        (this->*function)(vector+chunkSent, limit, parameter);
      }
      //last chunk, may have 0 to 'limit' doubles
      (this->*function)(vector+((iterations)*limit), vecSize-(iterations*limit), parameter);
    }
  }

  void _arrayPow2(double* vector, size_t vecSize, double garbage) {
    // Data we are sending to the server. Copy values
    NumberArray in;
    for (int i = 0; i < vecSize; ++i) {
      in.add_value(vector[i]);
    }

    // Container for the data we expect from the server.
    NumberArray out;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->ArrayPow2(&context, in, &out);

    // Restore values
    for (int i = 0; i < vecSize; ++i) {
      vector[i] = out.value(i);
    }

    // Act upon its status. No treatment implemented
    if (status.ok()) {
      return;
    } else {
      return;
    }
  }

  void _arrayInc(double* vector, size_t vecSize, double garbage) {
    // Data we are sending to the server. Copy values
    NumberArray in;
    for (int i = 0; i < vecSize; ++i) {
      in.add_value(vector[i]);
    }

    // Container for the data we expect from the server.
    NumberArray out;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->ArrayInc(&context, in, &out);

    // Restore values
    for (int i = 0; i < vecSize; ++i) {
      vector[i] = out.value(i);
    }

    // Act upon its status. No treatment implemented
    if (status.ok()) {
      return;
    } else {
      return;
    }
  }

  void _arrayMultiplyBy(double* vector, size_t vecSize, double parameter) {
    // Data we are sending to the server. Copy values
    NumberArrayParameter in;
    in.set_parameter(parameter);
    for (int i = 0; i < vecSize; ++i) {
      in.add_value(vector[i]);
    }

    // Container for the data we expect from the server.
    NumberArray out;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->ArrayMultiplyBy(&context, in, &out);

    //Restore values
    for (int i = 0; i < vecSize; ++i) {
      vector[i] = out.value(i);
    }

    // Act upon its status. No treatment implemented
    if (status.ok()) {
      return;
    } else {
      return;
    }
  }

  std::unique_ptr<ArrayOperator::Stub> stub_;
};

// Thread function for thread 'tid' to fill 'n' values on array 'element'
void fillArrayCorrect(int tid, long n, double* element, int numThreads) {
  // Use C++11 Mersenne Twister's random number generator
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(-100,100);
  // Number of elements generated per thread
  int nPerThread = n/numThreads;
  // Starting index for this thread
  int index = tid * nPerThread;
  for ( int i = index; i < index + nPerThread; i ++) {
    element[i] = dis(gen);
  }
}

// Fill vector 'element' with size 'n' using 'numThreads' threads
void randomAllocatedVector (double* element, long n, int numThreads) {
  std::thread *th = new std::thread[numThreads];
  //spawn worker threads and join after the work is done
  for( int i = 0; i < numThreads ; i++) {
    th[i] = std::thread(fillArrayCorrect,i,n,element, numThreads);
  }
  for(int i = 0; i < numThreads; i++) {
    th[i].join();
  }
  delete [] th;
}

// Run one execution of application
// This function was created to make it easier to run 10 tests
//and take timing average and standard deviation
double runApplication (int& NUM_THREADS, 
                      double* vector, 
                      long& n, 
                      size_t& nPerThreads, 
                      int& opcode, 
                      double& parameter) 
{

  std::thread* clients = new std::thread[NUM_THREADS];
  
  //create random vectors
  randomAllocatedVector(vector, n, NUM_THREADS);

  //start timing
  struct timeval start, end;
  gettimeofday(&start, NULL);

  for (int i = 0; i < NUM_THREADS; ++i) {
    // launch thread with c++11's lambda function
    // lambda is defined as: [captured variables](parameters){function}
    clients[i] = std::thread([i, vector, nPerThreads, opcode, parameter](){
      ArrayOperatorClient greeter(grpc::CreateChannel(
      "localhost:50051", grpc::InsecureChannelCredentials()));
      // find out where this thread's vector part begins with
      //pointer arithmetic 
      double *localVector = vector + nPerThreads*i;
      // Based on 'opcode' choose operation
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
  // join clients after they're done
  for (int i = 0; i < NUM_THREADS; ++i) {
    clients[i].join();
  }

  //end timing
  gettimeofday(&end, NULL);
  double seconds = ((end.tv_sec  - start.tv_sec) * 1000000u + 
    end.tv_usec - start.tv_usec) / 1.e6;

  //avoid memory leaks
  delete [] clients;

  // print to have a notion of progress
  std::cout<<"."<<std::flush;

  return seconds;
}

int main(int argc, char** argv) {
  // Usage explanation
  if (argc < 5)
  {
    std::cout<<"Usage is:\n"<<argv[0]<<
      " <amount of numbers to generate>"<<
      " <amount of threads>" <<
      " <opcode = 0,1,2>" << 
      " <parameter>\n" <<
      " <optional: number of executions. default = 1>"
      " OP CODES:\n" <<
      "    0 - Increment by 1;\n" <<
      "    1 - Apply power of 2;\n" <<
      "    2 - Multiply by parameter;\n" <<
      "\nDISCLAIMER: Even though not every operation uses a parameter," <<
      " you have to specify one when using the 'number of executions'"<<
      " input.\n" <<
      std::endl;
    return 0;
  }

  if (!isdigit(argv[1][0]) || !isdigit(argv[2][0]) || !isdigit(argv[3][0]))
  {
    std::cerr<<"Arguments should be numbers!"<<std::endl;
    return 0;
  }
  // size of vector
  long n = atol(argv[1]);
  // number of threads
  int NUM_THREADS = atoi(argv[2]);
  // operation 
  int opcode = atoi(argv[3]);
  if (opcode < 0 || opcode > 2) {
    std::cerr<<"IMPOSSIBLE OPCODE!"<<std::endl;
    return 0;
  }

  // Parameter for parameterized operations
  double parameter = 4.0;
  if (isdigit(argv[4][0])){
    parameter = atof(argv[4]);
  }

  // How many times is execution repeated
  int repeat = 1;
  if (argc > 5) {
    if (isdigit(argv[5][0])){
      repeat = atoi(argv[5]);
    }
  }

  // Number of elements processed by each thread
  size_t nPerThreads = n/NUM_THREADS;
  // Vector to be operated
  double* vector = new double[n];
  // Execution times, for standard deviation calculation
  double* times = new double[repeat];
  // Mean time
  double meanTime = 0.0;
  // Run all executions
  for (int i = 0; i < repeat; ++i) {
    times[i] = runApplication(NUM_THREADS, vector, n, 
                              nPerThreads, opcode, parameter);
    meanTime += times[i]/(double)repeat;
  }
  // Calculate standard deviation
  double stdDev = 0.0;
  for (int i = 0; i < repeat; ++i) {
    stdDev = ((meanTime-times[i])*(meanTime-times[i]))/(double)repeat;
  }
  stdDev = std::sqrt(stdDev);
  // Log results to console
  std::cout<<"\nMean time: "<<meanTime<<" seconds"<<std::endl;
  std::cout<<"\nStandard Deviation: "<<stdDev<<" seconds"<<std::endl;
  // Write results to file
  char fname[80];
  FILE* fout;
  sprintf(fname,"times.log");
  fout = fopen(fname,"a");
  fprintf(fout, "vector size: %ld", n);
  fprintf(fout, " nThreads: %d opcode: %d", NUM_THREADS, opcode);
  fprintf(fout, " sdev: %f s \n", stdDev);
  fprintf(fout, " mean time: %f s \n", meanTime);
  fclose(fout);

  //avoid memory leaks
  delete [] vector;
  return 0;
}
