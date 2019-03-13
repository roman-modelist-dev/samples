//
// Created by arrayio on 3/12/19.
//

#include "sorter.hpp"

bitonic_node* sort_data_functor::bitonic_split(sort_data_functor* sorter, input_iterator begin, input_iterator last, int depth)
{
  size_t full_length = std::distance(begin, last);
  size_t half_length = full_length / 2;
  input_iterator half_begin = begin + half_length;
  auto& tuple = sorter->steps[depth];
  
  if(depth < 0)
    return nullptr;
  
  assert(depth >= 0);
  --depth;
  auto it = tuple.insert(tuple.end(),
                         new bitonic_node(
                           range(begin, last),
                           bitonic_split(sorter, begin,half_begin, depth),
                           bitonic_split(sorter, half_begin, last, depth)));
  return *it;
}

uint8_t sort_data_functor::power2_more_than(size_t length)
{
  size_t power = step_count(length);
  return 1 << power;
}

uint8_t sort_data_functor::step_count(size_t length)
{
  if(length == 0)
    return 0;
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

sort_data_functor::sort_data_functor(input_iterator begin_, input_iterator last_)
  : begin(begin_)
  , last(last_)
  , ext_buffer(power2_more_than(std::distance(begin, last)))
  , padding_length(ext_buffer.size() - std::distance(begin, last))
  , root_node(nullptr)
  , steps(step_count(std::distance(begin, last)))
{
  std::move(begin, last, ext_buffer.begin());
  size_t count = 1;
  std::for_each(steps.rbegin(), steps.rend(), [&count](auto& step){
    step.reserve(count);
    count *= 2;
  });
  root_node = bitonic_split(this, ext_buffer.begin(), ext_buffer.end(), steps.size() - 1);
}

sort_data_functor::~sort_data_functor()
{
  if (root_node)
    delete root_node;
}

void sort_tuple_functor::operator()()
{
  constexpr const int odd_mask = 0x01;
  for (int depth = 0; depth < step; ++depth)
  {
    int acsending = 1;
    result_list results;
    std::for_each(tuple.begin(), tuple.end(), [&results, &acsending, &depth](auto& node)
    {
      switch (acsending)
      {
        case 1:
        {
          sort_bitonic<order_te::up> sort_op(node);
          results = sort_op(depth);
        } break;
        case 0:
        {
          sort_bitonic<order_te::down> sort_op(node);
          results = sort_op(depth);
        } break;
        default:
          assert(false);
      }
      ++acsending;
      acsending &= odd_mask;
    });
    
    for (auto& res: results)
      res.wait();
  }
}

void sort_data_functor::operator()()
{
  size_t count = 1;
  std::for_each(steps.begin(), steps.end(), [&count](auto& step){
    sort_tuple_functor func(step, count);
    func();
    ++count;
  });
  std::move(ext_buffer.begin() + padding_length, ext_buffer.end(), begin);
}
