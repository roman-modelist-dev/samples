//
// Created by arrayio on 3/12/19.
//

#ifndef TEST_SORT_SORTER_HPP
#define TEST_SORT_SORTER_HPP

#include "range.hpp"
#include "thread_pool.hpp"

#include <functional>
#include <vector>
#include <list>

using boost::sort::common::range;

using result_op = std::future<void>;
using result_list = std::list<result_op>;

//using string_list = std::vector<std::string>;
using string_list = std::vector<int>;
using input_iterator = string_list::iterator;
using range_data = range<input_iterator>;

using sort_pair = std::pair<input_iterator, input_iterator>;
using sort_data = std::vector<sort_pair>;

struct bitonic_node
{
  bitonic_node(): left(nullptr), right(nullptr) {}
  bitonic_node(range_data&& range_, bitonic_node* left_, bitonic_node* right_)
    : data()
    , left(left_)
    , right(right_)
  {
    assert( (left != nullptr && right != nullptr) || (left == nullptr && right == nullptr) );
    constexpr int even_mask = 0x01;
    if (even_mask & range_.size())
      data.reserve(range_.size()/2+1);
    else
      data.reserve(range_.size()/2);
    
    size_t full_length = std::distance(range_.first, range_.last);
    size_t half_length = full_length / 2;
    input_iterator current_it = range_.first;
    input_iterator half_begin = range_.first + half_length;
    for(;current_it != half_begin; ++current_it) {
      input_iterator pair_it = current_it + half_length;
      if( pair_it != range_.last )
        data.insert(data.end(), sort_pair(current_it, pair_it));
    }
  }
  ~bitonic_node()
  {
    assert( (left != nullptr && right != nullptr) || (left == nullptr && right == nullptr) );
    if (left)
      delete left;
    if (right)
      delete right;
  }
  bool has_branches() const
  {
    return (bool)(left && right);
  }
  sort_data     data;
  bitonic_node* left;
  bitonic_node* right;
};

using bitonic_tulpe = std::vector<bitonic_node*>;
using step_list = std::vector<bitonic_tulpe>;

struct sort_tuple_functor
{
  bitonic_tulpe &tuple;
  int step;
  
  sort_tuple_functor(bitonic_tulpe &tuple_, int step_)
    : tuple(tuple_), step(step_) {}
  
  void operator()();
};

struct sort_data_functor
{
  sort_data_functor(input_iterator begin_, input_iterator last_);
  ~sort_data_functor();
  void operator()();
private:
  using value_type = input_iterator::value_type;
  input_iterator begin;
  input_iterator last;
  std::vector<value_type> ext_buffer;
  size_t padding_length;
  bitonic_node* root_node;
  step_list steps;
  static uint8_t power2_more_than(size_t length);
  static uint8_t step_count(size_t length);
  static bitonic_node* bitonic_split(sort_data_functor* sorter, input_iterator begin, input_iterator last, int depth);
};

enum struct order_te
{
  up,
  down
};

template <order_te order>
struct check_and_swap
{
  input_iterator left;
  input_iterator right;
  check_and_swap(input_iterator left_, input_iterator right_)
    : left(left_)
    , right(right_)
  {}
  void operator()()
  {
    switch (order)
    {
      case order_te::up:
        if (*left > *right)
          std::swap(*left, *right);
        break;
      case order_te::down:
        if (*left < *right)
          std::swap(*left, *right);
        break;
      default:
        assert(false);
    }
  }
};

template <order_te order>
struct sort_bitonic
{
  bitonic_node* data;
  sort_bitonic(bitonic_node* data_): data(data_), m_thread_pool(thread_pool::instance()) {}
  result_list operator()(size_t depth)
  {
    return get_operations(data, depth);
  }

private:
  const thread_pool& m_thread_pool;
  
  result_list get_operations(bitonic_node* current, size_t depth)
  {
    if (depth == 0)
    {
      return get_current_operations(current);
    }
    else
    {
      --depth;
      result_list result;
      if(current->has_branches())
      {
        result.splice(result.end(), get_operations(current->left, depth));
        result.splice(result.end(), get_operations(current->right, depth));
      }
      return result;
    }
  }
  result_list get_current_operations(bitonic_node* current)
  {
    result_list result;
    std::for_each(current->data.begin(), current->data.end(), [&result, this](auto& pair)
    {
      result.push_back(m_thread_pool.exec(check_and_swap<order>(pair.first, pair.second)));
    });
    return result;
  }
};

#endif //TEST_SORT_SORTER_HPP
