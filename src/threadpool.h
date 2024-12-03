/******************************************************************************
 *
 * Copyright (C) 1997-2020 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby
 * granted. No representations are made about the suitability of this software
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * Documents produced by Doxygen are derivative works derived from the
 * input used in their production; they are not affected by this license.
 *
 */

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <condition_variable>
#include <deque>
#include <functional>
#include <future>
#include <mutex>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

/// Class managing a pool of worker threads.
/// Work can be queued by passing a function to queue(). A future will be
/// returned that can be used to obtain the result of the function after execution.
///
/// Usage example:
/// @code
/// ThreadPool pool(10);
/// std::vector< std::future< int > > results;
/// for (int i=0;i<10;i++)
/// {
///   auto run = [](int i) { return i*i; };
///   results.emplace_back(pool.queue(std::bind(run,i)));
/// }
/// for (auto &f : results)
/// {
///   printf("Result %d:\n", f.get());
/// }
/// @endcode
class ThreadPool
{
  public:
    /// start N threads in the thread pool.
    ThreadPool(std::size_t N=1)
    {
      for (std::size_t i = 0; i < N; ++i)
      {
        // each thread is a std::async running thread_task():
        m_finished.push_back(
            std::async(
              std::launch::async,
              [this]{ threadTask(); }
              )
            );
      }
    }
    /// deletes the thread pool by finishing all threads
    ~ThreadPool()
    {
      finish();
    }
    ThreadPool(const ThreadPool &) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;
    ThreadPool(ThreadPool &&) = delete;
    ThreadPool &operator=(ThreadPool &&) = delete;

    /// Queue the callable function \a f for the threads to execute.
    /// A future of the return type of the function is returned to capture the result.
    template<class F, typename ...Args>
    auto queue(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
    {
      // We wrap the function object into a packaged task, splitting
      // execution from the return value.
      // Since the packaged_task object is not copyable, we create it on the heap
      // and capture it via a shared pointer in a lambda and then assign that lambda
      // to a std::function.
      using RetType = decltype(f(args...));
      auto ptr = std::make_shared< std::packaged_task<RetType()> >(std::forward<F>(f), std::forward<Args>(args)...);
      auto taskFunc = [ptr]() { if (ptr->valid()) (*ptr)(); };

      auto r=ptr->get_future(); // get the return value before we hand off the task
      {
        std::unique_lock<std::mutex> l(m_mutex);
        m_work.emplace_back(taskFunc);
        m_cond.notify_one(); // wake a thread to work on the task
      }

      return r; // return the future result of the task
    }

    /// finish enques a "stop the thread" message for every thread,
    /// then waits for them to finish
    void finish()
    {
      {
        std::unique_lock<std::mutex> l(m_mutex);
        for(auto&& u : m_finished)
        {
          (void)u; //unused_variable, to silence the compiler warning about unused variables
          m_work.emplace_back(); // insert empty function object to signal abort
        }
      }
      m_cond.notify_all();
      m_finished.clear();
    }
  private:

    // the work that a worker thread does:
    void threadTask()
    {
      while(true)
      {
        // pop a task off the queue:
        std::function<void()> f;
        {
          // usual thread-safe queue code:
          std::unique_lock<std::mutex> l(m_mutex);
          if (m_work.empty())
          {
            m_cond.wait(l,[&]{return !m_work.empty();});
          }
          f = std::move(m_work.front());
          m_work.pop_front();
        }
        // if the function is empty, it means we are asked to abort
        if (!f) return;
        // run the task
        f();
      }
    }

    // the mutex, condition variable and deque form a single
    // thread-safe triggered queue of tasks:
    std::mutex m_mutex;
    std::condition_variable m_cond;

    // hold the queue of work
    std::deque< std::function<void()> > m_work;

    // this holds futures representing the worker threads being done:
    std::vector< std::future<void> > m_finished;
};

#endif

