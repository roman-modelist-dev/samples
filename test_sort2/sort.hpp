//
// Created by arrayio on 3/17/19.
//

#ifndef TEST_SORT_SORT_HPP
#define TEST_SORT_SORT_HPP

#include <vector>
#include <list>
#include <algorithm>
#include <type_traits>
#include <thread>
#include <future>
#include <cstddef>
#include <boost/sort/common/range.hpp>

using namespace boost::sort::common;

template <typename Iterator>
void parallel_merge_sort(Iterator begin, Iterator last) {
  auto thread_num = std::thread::hardware_concurrency();
  
  for(auto count = thread_num; count >=1; count = count/2)
  {
    size_t block_size = std::distance(begin, last) / count ;
    std::vector<range<Iterator>> blocks(count);
    auto it = begin;
  
    for(int i = 0; i < count; ++i)
    {
      blocks[i] = range<Iterator>(it, it + block_size);
      it += block_size;
    }
  
    std::vector<std::future<void>> results(blocks.size());
    size_t i = 0;
    std::for_each(blocks.begin(), blocks.end(), [&results, &i](auto& range){
      results[i] = std::async(std::launch::async, [](auto begin, auto last)
      {
        std::sort(begin, last);
      }, range.first, range.last);
      ++i;
    });
  
    for (auto& res: results)
      res.wait();
  }
}

#endif //TEST_SORT_SORT_HPP
