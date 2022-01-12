# ThreadPoolC++
ThreadPoolC++ is a simple and easy to use C++ threadpool implementation. It is just simple working POC, which can adjusted according to the use case.  

Keywords: threadpool, C++17, thread-safe, concurrent

## Build instructions
Use git to clone the repository.
```Shell
git clone https://github.com/JakubFornadel/ThreadPoolCpp.git
cd ThreadPoolCpp
```

A CMake configuration file is provided for multiplatform support.

```Shell
mkdir build
cd build

cmake ../
make

# run example
./ThreadPoolExample

# go back to the project root
cd ..
```

## Usage
```C++
#include <iostream>
#include <cstdlib>
#include <signal.h>
#include <chrono>

#include "ThreadPool.hpp"

bool stopProgram = false;

void stopListener(int sigNum) {
  std::cout << "Caught ctrl+c signal. Stop program." << std::endl;
  stopProgram = true;
};

int main() {
  std::cout << "ThreadPool example started" << std::endl;

  // Register ctrl+c signal and signal handler
  signal(SIGINT, stopListener);

  // Creates simple thread pool
  concurrent::ThreadPool<std::string> threadPool( [](const auto& taskData) { std::cout << "Processing task: " << taskData << std::endl; } );
  
  // Generate random tasks until ctrl+c signal received
  while (stopProgram == false) {
    threadPool.emplace("Hello world task");
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  threadPool.stopProcessing();

  std::cout << "Program finished" << std::endl;
}
```
