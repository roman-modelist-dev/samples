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

template <typename T>
using input_list = std::vector<T>;

template <typename T>
using input_iterator = typename input_list<T>::iterator;

template <typename T>
using range_data = range<input_iterator<T>>;

template <typename T>
using sort_pair = std::pair<input_iterator<T>, input_iterator<T>>;

template <typename T>
using sort_data = std::vector<sort_pair<T>>;

template <typename T>
struct bitonic_node
{
  bitonic_node(): left(nullptr), right(nullptr) {}
  bitonic_node(range_data<T>&& range_, bitonic_node<T>* left_, bitonic_node<T>* right_)
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
    input_iterator<T> current_it = range_.first;
    input_iterator<T> half_begin = range_.first + half_length;
    for(;current_it != half_begin; ++current_it) {
      input_iterator<T> pair_it = current_it + half_length;
      if( pair_it != range_.last )
        data.insert(data.end(), sort_pair<T>(current_it, pair_it));
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
  sort_data<T>     data;
  bitonic_node<T>* left;
  bitonic_node<T>* right;
};

template <typename T>
using bitonic_tulpe = std::vector<bitonic_node<T>*>;

template <typename T>
using step_list = std::vector<bitonic_tulpe<T>>;

enum struct order_te
{
  up,
  down
};

template <order_te order, typename T>
struct check_and_swap
{
  input_iterator<T> left;
  input_iterator<T> right;
  check_and_swap(input_iterator<T> left_, input_iterator<T> right_)
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

template <order_te order,typename T>
struct sort_bitonic
{
  bitonic_node<T>* data;
  sort_bitonic(bitonic_node<T>* data_): data(data_), m_thread_pool(thread_pool::instance()) {}
  result_list operator()(size_t depth)
  {
    return get_operations(data, depth);
  }

private:
  const thread_pool& m_thread_pool;
  
  result_list get_operations(bitonic_node<T>* current, size_t depth)
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
  result_list get_current_operations(bitonic_node<T>* current)
  {
    result_list result;
    std::for_each(current->data.begin(), current->data.end(), [&result, this](auto& pair)
    {
      result.push_back(m_thread_pool.exec(check_and_swap<order, T>(pair.first, pair.second)));
    });
    return result;
  }
};

template <typename T>
struct sort_tuple_functor
{
  bitonic_tulpe<T> &tuple;
  int step;
  
  sort_tuple_functor(bitonic_tulpe<T> &tuple_, int step_)
    : tuple(tuple_), step(step_) {}
  
  void operator()()
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
            sort_bitonic<order_te::up, T> sort_op(node);
            results = sort_op(depth);
          } break;
          case 0:
          {
            sort_bitonic<order_te::down, T> sort_op(node);
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
};

uint8_t power2_more_than(size_t length);
uint8_t step_count(size_t length);

template <typename Iterator>
struct sort_data_functor
{
  sort_data_functor(Iterator begin_, Iterator last_)
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
  
  ~sort_data_functor()
  {
    if (root_node)
      delete root_node;
  }
  void operator()()
  {
    size_t count = 1;
    std::for_each(steps.begin(), steps.end(), [&count](auto& step){
      sort_tuple_functor func(step, count);
      func();
      ++count;
    });
    std::move(ext_buffer.begin() + padding_length, ext_buffer.end(), begin);
  }

private:
  using value_type = typename Iterator::value_type;
  using buffer_iterator = typename std::vector<value_type>::iterator;
  using bitonoc_node_t = bitonic_node<value_type>;
  Iterator begin;
  Iterator last;
  std::vector<value_type> ext_buffer;
  size_t padding_length;
  bitonoc_node_t* root_node;
  step_list<value_type> steps;
  static bitonoc_node_t* bitonic_split(sort_data_functor* sorter, buffer_iterator begin, buffer_iterator last, int depth)
  {
    size_t full_length = std::distance(begin, last);
    size_t half_length = full_length / 2;
    buffer_iterator half_begin = begin + half_length;
    auto& tuple = sorter->steps[depth];
  
    if(depth < 0)
      return nullptr;
  
    assert(depth >= 0);
    --depth;
    auto it = tuple.insert(tuple.end(),
                           new bitonoc_node_t(
                             range(begin, last),
                             bitonic_split(sorter, begin,half_begin, depth),
                             bitonic_split(sorter, half_begin, last, depth)));
    return *it;
  }
};


#endif //TEST_SORT_SORTER_HPP
