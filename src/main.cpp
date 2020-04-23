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
  concurrent::ThreadPool<std::string> threadPool( [](auto taskData) { std::cout << "Processing task: " << taskData << std::endl; } );
  
  // Generate random tasks until ctrl+c signal received
  while (stopProgram == false) {
    threadPool.emplace("Hello world task");
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  threadPool.stopProcessing();

  std::cout << "Program finished" << std::endl;
}
