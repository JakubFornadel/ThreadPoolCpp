#ifndef _THREAD_POOL_
#define _THREAD_POOL_

#include <iostream>
#include <vector>
#include <queue>
#include <chrono>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace concurrent {

/**
 * @brief Simple ThreadPool class that might be used for procssing tasks. It is just simple working POC, which
 *        would be adjusted according to the use case.  
 **/ 
template <typename Task>
class ThreadPool {
public:
  // Custom processor function signature
  using Processor = std::function<void(Task&)>;

  // Ctors and Dtors
  /**
   * @param processor   Custom processingfunction
   * @param workersNum  Number of workers
   **/
  ThreadPool(Processor&& processor, size_t workersNum = 5) :
    stopProcessing_(false),
    ignoreExistingTasks_(false),
    processor_(std::move(processor)),
    queue_(),
    mutex_(),
    cond_var_(),
    workers_()
  {
    try {
      for (size_t idx = 0; idx < workersNum; idx++) {
        workers_.emplace_back(&ThreadPool::processTask, this, idx);
      }  
    }
    catch (...) {
      stopProcessing_ = true;
      ignoreExistingTasks_ = true;

      throw;
    }
  };


  ~ThreadPool() {
    stopProcessing(true);

    for (auto& worker : workers_) {
      if (worker.joinable() == true) {
        worker.join();
      }
    }
  }

  ThreadPool(const ThreadPool&)             = delete;
  ThreadPool& operator= (const ThreadPool&) = delete;
  ThreadPool(ThreadPool&&)                  = delete;
  ThreadPool& operator= (ThreadPool&&)      = delete;

  /**
   * @brief Pushes the given element value to the end of the queue. Used for l-values
   **/ 
  void push(const Task& value) {
      if (stopProcessing_ == true) {
        return;
      }

      std::scoped_lock lock(mutex_);

      queue_.push(value);
      cond_var_.notify_one();
  }
  
  /**
   * @brief Pushes the given element value to the end of the queue. Used for r-values
   **/ 
  void push(Task&& value) {
      if (stopProcessing_ == true) {
        return;
      }

      std::scoped_lock lock(mutex_);

      queue_.push(std::move(value));
      cond_var_.notify_one();
  }
  
  /**
   * @brief Creates new element in the queue using perfect-forwarding method.
   **/ 
  template<typename... Args>
  void emplace(Args&&... args) {
      if (stopProcessing_ == true) {
        return;
      }

      std::scoped_lock lock(mutex_);

      queue_.emplace(std::forward<Args>(args)...);
      cond_var_.notify_one();
  }

  void stopProcessing(bool ignoreExistingTasks = true) {
    stopProcessing_ = true;
    ignoreExistingTasks_ = true;

    cond_var_.notify_all();
  }

  /**
   * @brief Threadpool sycnchronized processing function, which calls user-defined custom processing function
   **/
  void processTask(size_t workerId) {
    std::cout << "Worker num " << workerId << " started" << std::endl;
    std::unique_lock<std::mutex> lock(mutex_, std::defer_lock);
    
    while (ignoreExistingTasks_ == false) {
      lock.lock();

      while (queue_.empty() == true) {
        if (stopProcessing_ == true) {
          std::cout << "Worker num " << workerId << ": finished" << std::endl;
          return;
        }

        cond_var_.wait(lock);
      } 
      
      Task task = std::move(queue_.front());
      queue_.pop();

      lock.unlock();

      std::cout << "Worker num " << workerId << ": ";

      // Calls custom processor
      processor_(task);
    }
  }

private:
  // If true, stop processing new tasks, but finish existing ones
  bool                       stopProcessing_;
  // If ignoreExistingTasks_ == true && stopProcessing_ == true, stop processing immediately and ignore new & existing tasks
  bool                       ignoreExistingTasks_;

  // Custom processing function
  Processor                  processor_;

  // Queue of unprocessed tasks
  std::queue<Task>           queue_;
  // Queue mutex
  std::mutex                 mutex_;
  // Queue condition variable
  std::condition_variable    cond_var_;
  
  // Vector of worker threads - should be initialized as the last member
  std::vector<std::thread>   workers_;
};

}

#endif