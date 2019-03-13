//
// Created by arrayio on 3/12/19.
//

#include "sorter.hpp"

bitonic_node* sort_data_functor::bitonic_split(sort_data_functor* sorter, input_iterator begin, input_iterator last, int depth)
{
  size_t full_length = std::distance(begin, last);
  if (full_length == 1)
    return nullptr_t;
  
  size_t half_length = full_length / 2;
  input_iterator half_begin = begin + half_length;
  auto& tuple = sorter->steps[depth];
  
  ++depth;
  auto it = tuple.insert(bitonic_node(
    range(begin, last),
    bitonic_split(begin,half_begin, depth),
    bitonic_split(half_begin, last, depth)
  ));
  return &(*it);
}

uint8_t sort_data_functor::step_count(size_t length)
{
  int count = 64;
  constexpr size_t mask = 0x8000000000000000;
  for (int i = 0 ; i < 64; ++i)
  {
    auto res = length & mask;
    if (res == mask)
      break;
    --count;
    length = length << 1;
  }
  length = length << 1;
  if (length == 0)
    return --count;
  return count;
}

sort_data_functor::sort_data_functor(input_iterator begin, input_iterator last)
  :steps(step_count(std::distance(begin, last)))
{

}

void sort_tuple_functor::operator()()
{
  int acsending = 1;
  constexpr const int odd_mask = 0x01;
  for (int depth = step; depth >= 0; --depth)
  {
    result_list results;
    std::for_each(tuple.begin(), tuple.end(), [&result](auto& node)
    {
      switch (acsending)
      {
        case 1:
          sort_bitonic<true> sort_op(&node);
          results = sort_op(depth);
          break;
        case 0:
          sort_bitonic<false> sort_op(&node);
          results = sort_op(depth);
          break;
      }
      ++acsending;
      acsending &= odd_mask;
    });
    
    for (auto& res: results)
      res.wait();
  }
}