//
// Created by arrayio on 3/12/19.
//

#ifndef TEST_SORT_SORTER_HPP
#define TEST_SORT_SORTER_HPP

#include "range.hpp"
#include "thread_pool.hpp"

#include <functional>

using result_op = std::future<void>;
using result_list = std::list<result_op>;

using string_list = std::vector<std::string>;
using input_iterator = string_list::iterator;
using range_data = range<input_iterator>;

using operation_list = std::vector<swap_operation>;

using boost::sort::common::range;

using sort_pair = std::pair<input_iterator, input_iterator>;
using sort_data = std::vector<sort_pair>;

struct bitonic_node
{
  bitonic_node(): left_node(nullptr), right_node(nullptr) {}
  bitonic_node(range_data&& range_, bitonic_node* left_, bitonic_node* right_)
    : data()
    , left(left_)
    , right(right_)
  {
    assert( (left != nullptr && right != nullptr) || (left == nullptr && right == nullptr) );
    data.reserve(range_.size()/2+1);
    
    size_t full_length = std::distance(range_.first, range_.last);
    size_t half_length = full_length / 2;
    input_iterator current_it = range_.first;
    input_iterator half_begin = range_.first + half_length;
    for(;current_it != half_begin; ++current_it) {
      input_iterator pair_it = current_it + half_length;
      if( pair_it != current->last )
        data.insert(sort_pair(current_it, pair_it));
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
  sort_data_functor(input_iterator begin, input_iterator last);
  step_list steps;
private:
  static uint8_t step_count(size_t length);
  static bitonic_node* bitonic_split(sort_data_functor* sorter, input_iterator begin, input_iterator last, int depth = 0);
};

template <bool acsending>
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
    switch (acsending)
    {
      case true:
        if (*left > *right)
          std::swap(*left, *right);
        break;
      case false:
        if (*left < *right)
          std::swap(*left, *right);
        break;
    }
  }
};

template <bool ascending>
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
  
  static result_list get_operations(bitonic_node* current, size_t depth)
  {
    if (depth == 0)
    {
      return get_current_operations(current);
    }
    else
    {
      --depth;
      result_list result;
      if(data->has_branches())
      {
        result.insert(result.end(), *this(data->left, depth));
        result.insert(result.end(), *this(data->right, depth));
      }
      return result;
    }
  }
  static result_list get_current_operations(bitonic_node* current)
  {
    assert(current->begin != current.last);
    size_t full_length = std::distance(current->begin, current->last);
    if ( full_length <= 1 )
      return operation_list();
    
    size_t half_length = full_length / 2;
    input_iterator current_it = begin;
    input_iterator half_begin = begin + half_length;
    result_list result;
    for(;current_it != half_begin; ++current_it)
    {
      input_iterator pair_it = current_it + half_length;
      if( pair_it != current->last )
        result.append(m_thread_pool.exec(check_and_swap<ascending>(current_it, pair_it)));
    }
    return result;
  }
};

#endif //TEST_SORT_SORTER_HPP
