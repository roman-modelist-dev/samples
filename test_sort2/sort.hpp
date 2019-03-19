//
// Created by arrayio on 3/17/19.
//

#ifndef TEST_SORT_SORT_HPP
#define TEST_SORT_SORT_HPP

#include <vector>
#include <list>
#include <algorithm>
#include <type_traits>
#include <cstddef>

#include "thread_pool.hpp"

size_t step_count(size_t length);

template <typename Iterator_in, typename Iterator_out>
void merge(Iterator_in first_begin, Iterator_in first_end, Iterator_in second_begin, Iterator_in second_end, Iterator_out out)
{
  Iterator_in first_it = first_begin;
  Iterator_in second_it = second_begin;
  while (first_it != first_end && second_it != second_end)
  {
    if (*first_it < *second_it)
    {
      *out = *first_it;
      ++first_it;
    }
    else
    {
      *out = *second_it;
      ++second_it;
    }
    ++out;
  }
  if (first_it != first_end)
    std::copy(first_it, first_end, out);
  if (second_it != second_end)
    std::copy(second_it, second_end, out);
};

size_t count_merges(size_t length);

template <typename Iterator>
void parallel_merge_sort(Iterator begin, Iterator last)
{
  using value_type = typename Iterator::value_type;
  using buffer_t = std::vector<value_type>;
  using buffer_pair_t = std::pair<buffer_t&, buffer_t&>;

  //auto& thread_pool = thread_pool_t::instance();
  
  size_t length = std::distance(begin, last);
  buffer_t buffer1(length);
  buffer_t buffer2(length);
  
  auto buffer_pair = buffer_pair_t(buffer1, buffer2);
  std::copy(begin, last, buffer1.begin());
  
  auto switch_buffers = [](const buffer_pair_t& buffer_pair_)
  {
    return buffer_pair_t(buffer_pair_.second, buffer_pair_.first);
  };
  
  size_t factor = 2; //length of merged arrays
  size_t num_step = step_count(length);
  //size_t merge_num = count_merges(length);
  
  for(int i = 0; i < num_step; ++i)
  {
    auto& in_buf = buffer_pair.first;
    auto& out_buf = buffer_pair.second;
    auto out_it = out_buf.data();
    auto left_it = in_buf.data();
    const auto in_buf_end = in_buf.data() + in_buf.size();
    auto right_it = in_buf.data() + factor/2;
    
    //constexpr size_t max_operations_num = 0x8000 * 4;
    //auto operation_num = std::min(merge_num, max_operations_num);
    
    
    for(; left_it < in_buf_end ;)
    {
//      do
//      {
//        auto res = thread_pool.exec([=]()
//        {
         merge(std::min(left_it, in_buf_end),  std::min((left_it + factor/2), in_buf_end ),
               std::min(right_it, in_buf_end), std::min((right_it + factor/2), in_buf_end ),
               out_it);
//        });
//        if(!res)
 //       {
 //         thread_pool.wait_all();
 //         continue;
 //       }
  //      break;
  //    } while (true);
      left_it += factor;
      right_it += factor;
      out_it += factor;
    }
//    thread_pool.wait_all();
    buffer_pair = switch_buffers(buffer_pair);
    factor *= 2;
    //merge_num = count_merges(merge_num);
  }
  auto& result_buf = buffer_pair.first;
  std::copy(result_buf.begin(), result_buf.end(), begin);
}

#endif //TEST_SORT_SORT_HPP
