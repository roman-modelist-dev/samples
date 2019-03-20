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

size_t step_count(size_t length);
size_t power2_more_than(size_t length);

template <typename Iterator_in, typename Iterator_out>
void merge(Iterator_in first_begin, Iterator_in first_end, Iterator_in second_begin, Iterator_in second_end, Iterator_out out)
{
  Iterator_in first_it = first_begin;
  Iterator_in second_it = second_begin;
  while (first_it != first_end && second_it != second_end)
  {
    if (*first_it < *second_it)
    {
      *out = (*first_it);
      ++first_it;
    }
    else
    {
      *out = (*second_it);
      ++second_it;
    }
    ++out;
  }
  if (first_it != first_end)
    std::move(first_it, first_end, out);
  if (second_it != second_end)
    std::move(second_it, second_end, out);
};

size_t count_merges(size_t length);

using namespace boost::sort::common;

template <typename Iterator>
void parallel_merge_sort(Iterator begin, Iterator last) {
  using value_type = typename Iterator::value_type;
  using buffer_t = std::vector<value_type>;
  using range_t = range<typename buffer_t::iterator>;
  struct buffer_pair_t
  {
    buffer_pair_t(buffer_t* in, buffer_t* out):in_buf(in), out_buf(out){}
    void switch_pair()
    {
      auto* tmp = out_buf;
      out_buf = in_buf;
      in_buf = tmp;
    }
    buffer_t* in_buf;
    buffer_t* out_buf;
  };

  size_t length = std::distance(begin, last);
  auto num_of_threads = power2_more_than(std::thread::hardware_concurrency());
  //TODO: it may be improve - num of threads may reduce in some cases for best perfomance
  
  if (length < 0x100 or num_of_threads < 2)
  {
    std::sort(begin,last);
    return;
  };
  
  buffer_t buffer1(length);
  buffer_t buffer2(length);
  
  std::move(begin, last, buffer1.begin());
  
  auto buffer_pair = buffer_pair_t(&buffer1, &buffer2);
  size_t block_size = length / num_of_threads;
  
  std::vector<range_t> blocks(num_of_threads);
  
  auto it = buffer1.begin();
  
  for(int i = 0; i < num_of_threads; ++i)
  {
    if( i + 1 == num_of_threads)
      blocks[i] = range_t(it, last);
    else
      blocks[i] = range_t(it, it + block_size);
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
  
  size_t factor = 1; //length of merged arrays
  size_t num_step = step_count(blocks.size());
  
  for(int i = 0; i < num_step; ++i)
  {
    auto& in_buf = *buffer_pair.in_buf;
    auto& out_buf = *buffer_pair.out_buf;
    auto out_it = out_buf.data();
    auto left_it = in_buf.data();
    const auto in_buf_end = in_buf.data() + in_buf.size();
    auto right_it = in_buf.data() + block_size * factor;
    
    std::list<std::future<void>> results;
    
    for(int count = 0; count <  (num_of_threads / (factor * 2))  ; ++count)
    {
      assert(left_it < in_buf_end);
      results.push_back(std::async(std::launch::async, [=]()
      {
        auto right_end = ( count+1 ==  num_of_threads / (factor * 2) )? in_buf_end: std::min((right_it + block_size * factor), in_buf_end );
        merge(std::min(left_it, in_buf_end),  std::min((left_it + block_size * factor), in_buf_end ),
              std::min(right_it, in_buf_end), right_end,
              out_it);
      }));
      left_it += block_size * factor * 2;
      right_it += block_size * factor * 2;
      out_it += block_size * factor * 2;
    }
    for (auto& res: results)
      res.wait();
    buffer_pair.switch_pair();
    factor *= 2;
  }
  buffer_pair.switch_pair();
  auto& result_buf = *buffer_pair.out_buf;
  std::move(result_buf.begin(), result_buf.end(), begin);
}

#endif //TEST_SORT_SORT_HPP
