//
// Created by arrayio on 3/13/19.
//

#ifndef TEST_SORT_THREAD_POOL_HPP
#define TEST_SORT_THREAD_POOL_HPP

#include <future>
#include <list>
#include <atomic>
#include <boost/asio/io_service.hpp>
#include <boost/asio/post.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <boost/interprocess/sync/spin/mutex.hpp>
#include <boost/interprocess/sync/spin/semaphore.hpp>

#include <boost/lockfree/queue.hpp>

using boost::interprocess::ipcdetail::spin_mutex;
using boost::interprocess::ipcdetail::spin_semaphore;
using boost::interprocess::interprocess_semaphore;

class spin_result_t
{
  std::atomic<bool> m_result;
public:
  spin_result_t(): m_result(false) {}
  void final_notify()
  {
    m_result = true;
  }
  void wait()
  {
    while(m_result.load() == false)
    {
      continue;
    }
  }
};


class task_manager;

/*
class semaphore
{
  std::atomic<size_t>     m_count;
//  std::mutex              m_mutex_empty;
//  std::condition_variable m_cwait_empty;
  std::mutex              m_mutex_non_empty;
  std::condition_variable m_cwait_non_empty;
  std::unique_lock<std::mutex> m_lock_non_empty;
  task_manager*           m_task_manager;
public:
  semaphore(task_manager* p_task_manager);
  
  bool acquire();
  bool wait_fot_null();
  void release();
};
*/
class task_base
{
public:
  virtual ~task_base(){}
  virtual void operator()() = 0;
};

class semaphore_queue
{
  boost::lockfree::queue<task_base*, boost::lockfree::capacity<0xffff-1>> m_task_queue;
  task_manager* m_task_manager;
  spin_semaphore m_semaphore;
  std::atomic<size_t> m_dbg_count;
public:
  semaphore_queue(task_manager* p_task_manager);
  ~semaphore_queue();
  using value_type = task_base*;
  bool wait_for_empty();
  bool empty() const;
  bool push(const value_type& p_task);
  bool pop(value_type& p_task);
};

using spin_result_ptr = std::shared_ptr<spin_result_t>;

template<typename Fn>
class task_t: public task_base
{
  Fn m_func;
//  spin_result_ptr m_final;
public:
  task_t(Fn&& f): m_func(f)/*, m_final(new spin_result_t())*/{}
  virtual ~task_t(){}
  /*spin_result_ptr spin_result()
  {
    return m_final;
  }*/
  
  virtual void operator()() override
  {
    m_func();
    //m_final->final_notify();
  }
};

class task_manager
{
public:
  task_manager();
  ~task_manager();

  bool is_stopped() const;
  bool is_current_thread() const;
  bool wait_for_done();
  bool add_task(task_base* p_task);
private:
  std::atomic<bool>  m_stop;
  spin_mutex         m_task_running;
  semaphore_queue    m_task_queue;
  std::thread        m_thread;
  
  task_base* get_task();
  void exec();

};

using thread_list_t = std::vector<task_manager>;

class thread_pool_t
{
  thread_pool_t();
  ~thread_pool_t();
  thread_list_t m_thread_list;
  size_t m_thread_num;
  size_t m_num_factor;
public:
  static thread_pool_t& instance()
  {
    static thread_pool_t pool;
    return pool;
  }
  
  void wait_all()
  {
    for(auto it = m_thread_list.begin(); it != m_thread_list.end(); ++it)
    {
      auto ind = std::distance(m_thread_list.begin(), it);
      auto res = it->wait_for_done();
      assert(res);
    }
  }
  
  size_t threads_num () const;
  
  template <typename Function_f>
  bool exec(Function_f&& f)
  {
    using func_f = typename std::remove_reference<decltype(f)>::type;
    auto* p_task = new task_t<func_f>(std::move(f));
    //spin_result_ptr res = p_task->spin_result();
    auto res = m_thread_list[m_thread_num].add_task(p_task);
    if(!res)
      delete p_task;
    
    ++m_num_factor;
    m_num_factor = m_num_factor % 8;
    if(m_num_factor == 0)
      ++m_thread_num;
    m_thread_num = m_thread_num % (std::thread::hardware_concurrency());
    return res;
  }
};


#endif //TEST_SORT_THREAD_POOL_HPP
