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
/// Work can be queued by passing a function to queue(). When the
/// work is done the result of the function will be passed back via a future.
///
/// Note that due to a bug in Visual Studio's std::packaged_task implementation
/// it does not allow m_work to have a void() template parameter, and still assign
/// R() to it (you will get C2280: "attempting to reference a deleted function error").
/// So to work around this we pass the return type with the class itself :-(
/// See also https://stackoverflow.com/q/26733430/784672
template<class R>
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

    /// Queue the lambda 'task' for the threads to execute.
    /// A future of the return type of the lambda is returned to capture the result.
    /// use this once the Visual Studio bug is fixed:
    ///    template<class F, class R=std::result_of_t<F&()> >
    template<class F>
    std::future<R> queue(F&& f)
    {
      // wrap the function object into a packaged task, splitting
      // execution from the return value:
      std::packaged_task<R()> p(std::forward<F>(f));

      auto r=p.get_future(); // get the return value before we hand off the task
      {
	std::unique_lock<std::mutex> l(m_mutex);
	m_work.emplace_back(std::move(p)); // store the task<R()> as a task<void()>
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
          unused_variable(u);
	  m_work.push_back({});
	}
      }
      m_cond.notify_all();
      m_finished.clear();
    }
  private:

    // helper to silence the compiler warning about unused variables
    template <typename ...Args>
    void unused_variable(Args&& ...args) { (void)(sizeof...(args)); }

    // the work that a worker thread does:
    void threadTask()
    {
      while(true)
      {
	// pop a task off the queue:
	std::packaged_task<R()> f;
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
	// if the task is invalid, it means we are asked to abort
	if (!f.valid()) return;
	// otherwise, run the task
	f();
      }
    }

    // the mutex, condition variable and deque form a single
    // thread-safe triggered queue of tasks:
    std::mutex m_mutex;
    std::condition_variable m_cond;

    // note that a packaged_task<void()> can store a packaged_task<R()> (but not with buggy Visual Studio)
    std::deque< std::packaged_task<R()> > m_work;

    // this holds futures representing the worker threads being done:
    std::vector< std::future<void> > m_finished;
};

#endif

