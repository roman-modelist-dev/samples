//
// Created by arrayio on 3/17/19.
//

#include <iostream>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include "thread_pool.hpp"
#include <boost/interprocess/sync/scoped_lock.hpp>

task_manager::task_manager()
  : m_stop(false)
  , m_task_queue(this)
  , m_thread(new std::thread(std::bind(&task_manager::exec, this)))
{
}

task_manager::~task_manager()
{
  m_stop = true;
  if(m_thread)
    m_thread->join();
}

void task_manager::restart()
{
  m_stop = true;
  if(m_thread) {
    m_thread->join();
    m_stop = false;
    m_thread.reset(new std::thread(std::bind(&task_manager::exec, this)));
  }
}

void task_manager::exec()
{
  while (!m_stop.load()||!m_task_queue.empty())
  {
    //boost::interprocess::scoped_lock<spin_mutex> lock(m_task_running);
    //std::lock_guard<std::mutex> lock(m_task_running);
    auto* task = get_task();
    if (task)
    {
      (*task)();
      delete task;
    }
    
  }
}

bool task_manager::add_task(task_base* p_task)
{
  return m_task_queue.push(p_task);
}

task_base* task_manager::get_task()
{
  auto lock_count = m_task_queue.empty();
  task_base* result = nullptr;
  if(m_task_queue.pop(result))
    return result;
  return nullptr;
}

thread_pool_t::thread_pool_t()
  : m_thread_list(std::thread::hardware_concurrency())
  , m_thread_num(0)
  , m_num_factor(0)
{
}

thread_pool_t::~thread_pool_t()
{}

size_t thread_pool_t::threads_num() const
{
  return m_thread_list.size();
}

semaphore_queue::semaphore_queue(task_manager* p_task_manager)
  : m_task_queue(0x100000)
  , m_task_manager (p_task_manager)
  , m_semaphore(0)
  , m_dbg_count(0)
{}

semaphore_queue::~semaphore_queue()
{
  while(!m_task_queue.empty())
  {
    task_base* result;
    m_task_queue.pop(result);
    delete result;
  }
}

bool semaphore_queue::empty() const
{
  return m_task_queue.empty();
}

bool semaphore_queue::wait_for_empty()
{
  while(!m_task_queue.empty()&&!m_task_manager->is_stopped())
    std::this_thread::yield();
  return m_task_queue.empty();
}

bool semaphore_queue::push(const value_type& p_task)
{
  auto res = m_task_queue.push(p_task);
  if(res)
  {
    m_semaphore.post();
    ++m_dbg_count;
  }
  return res;
}

bool semaphore_queue::pop(value_type& p_task)
{
  if(m_semaphore.timed_wait(boost::posix_time::microsec_clock::local_time() + boost::posix_time::millisec(500)))
  {
    --m_dbg_count;
    return m_task_queue.pop(p_task);
  }
  return false;
}

/*
semaphore::semaphore(task_manager* p_task_manager)
  : m_count(0)
  , m_lock_non_empty(m_mutex_non_empty)
  , m_task_manager(p_task_manager)
{}

bool semaphore::wait_fot_null()
{
  assert(m_task_manager->is_current_thread() == false);
//  std::unique_lock<std::mutex> lock(m_mutex_empty);
  bool stoped = false;
  while (!stoped )
  {
    if(m_count.load() > 0)
    {
      //std::cerr<<"empty wait, "<<std::this_thread::get_id()<<std::endl;
      //m_cwait_empty.wait(lock);
      stoped = m_task_manager->is_stopped();
      continue;
    }
    break;
  };
  return m_count.load() == 0;
}

bool semaphore::acquire()
{
  assert(m_task_manager->is_current_thread());
  bool stoped = false;
  while (!stoped )
  {
    if(m_count.load() == 0)
    {
//      std::cerr<<"acquire wait, "<<std::this_thread::get_id()<<std::endl;
//      m_cwait_non_empty.wait(m_lock_non_empty);
      stoped = m_task_manager->is_stopped();
      continue;
    }
    break;
  };
  if(stoped)
    return false;
 
//  if(m_count.load() == 1)
//  {
//    std::unique_lock<std::mutex> lock(m_mutex_empty);
//    --m_count;
//    std::cerr<<"acquire --m_count, "<<m_count.load()<<", "<<std::this_thread::get_id()<<std::endl;
//    m_cwait_empty.notify_all();
//    return true;
  }
  --m_count;
  return true;
  
}

void semaphore::release()
{
  assert(m_task_manager->is_current_thread() == false);
  if(m_count.load() == 0)
  {
 //   std::unique_lock<std::mutex> lock(m_mutex_non_empty);
    ++m_count;
//    std::cerr<<"release ++m_count, "<<m_count.load()<<", "<<std::this_thread::get_id()<<std::endl;
//    m_cwait_non_empty.notify_one();
    return;
  }
  ++m_count;
}
*/

bool task_manager::is_stopped() const
{
  return m_stop.load();
}

bool task_manager::is_current_thread() const
{
  if(m_thread)
    return std::this_thread::get_id() == m_thread->get_id();
  return false;
}
/*
bool task_manager::wait_for_done()
{
 // boost::interprocess::scoped_lock<spin_mutex> lock(m_task_running);
  std::lock_guard<std::mutex> lock(m_task_running);
  return m_task_queue.wait_for_empty();
}*/