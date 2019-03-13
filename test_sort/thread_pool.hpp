//
// Created by arrayio on 3/13/19.
//

#ifndef TEST_SORT_THREAD_POOL_HPP
#define TEST_SORT_THREAD_POOL_HPP

#include <future>

class thread_pool
{
  thread_pool();
  ~thread_pool();
public:
  static const thread_pool& instance()
  {
    static const thread_pool;
    return thread_pool;
  }
  
  template <typename Function_f>
  std::future<void> exec(Function_f&& f) const
  {
    std::packaged_task<void()> result(f);
    
    //TODO: need to implement thread pool
    std::async(std::launch::async, result);
    
    return result.get_future();
  }
};

#endif //TEST_SORT_THREAD_POOL_HPP
