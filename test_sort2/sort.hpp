//
// Created by arrayio on 3/17/19.
//

#ifndef TEST_SORT_SORT_HPP
#define TEST_SORT_SORT_HPP

#include <vector>
#include <type_traits>
#include <cstddef>

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

class sorter_t
{
public:
  sorter_t(){}
  ~sorter_t(){}
  
  template <typename Iterator>
  void operator()(Iterator begin, Iterator last)
  {
    using value_type = typename Iterator::value_type;
    using buffer_t = std::vector<value_type>;
    using buffer_pair_t = std::pair<buffer_t&, buffer_t&>;
  
    size_t length = std::distance(begin, last);
    buffer_t buffer1(length);
    buffer_t buffer2(length);
    
    auto buffer_pair = buffer_pair_t(buffer1, buffer2);
    std::copy(begin, last, buffer1.begin());
    
    auto switch_buffers = [this](const buffer_pair_t& buffer_pair_)
    {
      return buffer_pair_t(buffer_pair_.second, buffer_pair_.first);
    };
    
    size_t factor = 2; //length of merged arrays
    size_t num_step = step_count(length);
    
    for(int i = 0; i < num_step; ++i)
    {
      auto& in_buf = buffer_pair.first;
      auto& out_buf = buffer_pair.second;
      auto out_it = out_buf.begin();
      auto left_it = in_buf.begin();
      auto right_it = in_buf.begin() + factor/2;
      for(; left_it <= in_buf.end() && right_it <= in_buf.end(); )
      {
        merge(left_it, std::min((left_it + factor/2), in_buf.end()),
              right_it,std::min((right_it + factor/2), in_buf.end()),
              out_it
        );
        left_it += factor;
        right_it += factor;
        out_it += factor;
      }
      buffer_pair = switch_buffers(buffer_pair);
      factor *= 2;
    }
    auto& result_buf = buffer_pair.first;
    std::copy(result_buf.begin(), result_buf.end(), begin);
  }
};

#endif //TEST_SORT_SORT_HPP
