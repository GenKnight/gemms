//
// Created by harvey on 2017/5/9.
//

#pragma once

#include "iwkats_core.h"

#include <chrono>
#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

IWKATS_NAMESPACE_BEGIN

class CtpQryPool {
public:
  CtpQryPool(size_t = 1, std::chrono::seconds interval_sec = std::chrono::seconds(0));
  void enqueue(const std::function<int()>& task);
  ~CtpQryPool();
private:
  // need to keep track of threads so we can join them
  std::vector<std::thread> workers;
  // the task queue
  std::queue<std::function<int()> > tasks;

  // synchronization
  std::mutex queue_mutex;
  std::condition_variable condition;
  bool stop;
  std::chrono::seconds interval_ms;
};

// the constructor just launches some amount of workers
inline CtpQryPool::CtpQryPool(size_t threads, std::chrono::seconds interval)
  : stop(false), interval_ms(interval) {
  for(size_t i = 0; i < threads; ++i)
    workers.emplace_back(
      [this] {
    for(;;) {
      std::function<int()> task;
      {
        std::unique_lock<std::mutex> lock(this->queue_mutex);
        this->condition.wait(lock,
                             [this] { return this->stop || !this->tasks.empty(); });
        if(this->stop && this->tasks.empty())
          return;
        task = std::move(this->tasks.front());

        // 如果执行不成功，隔1s继续执行
        int ret = -1;
        while(0 != (ret = task())) {
          std::this_thread::sleep_for(interval_ms);
        }
        this->tasks.pop();
      }
    }
  }
  );
}

void CtpQryPool::enqueue(const std::function<int()>& task) {
  {
    std::unique_lock<std::mutex> lock(queue_mutex);
    if(stop) {
      return;
    }
    tasks.emplace(task);
  }
  condition.notify_one();
}

// the destructor joins all threads
inline CtpQryPool::~CtpQryPool() {
  {
    std::unique_lock<std::mutex> lock(queue_mutex);
    stop = true;
  }
  condition.notify_all();
  for(std::thread& worker : workers)
    worker.join();
}

IWKATS_NAMESPACE_END
