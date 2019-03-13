//
// Created by arrayio on 3/13/19.
//

#ifndef TEST_SORT_BITONIC_HPP
#define TEST_SORT_BITONIC_HPP

template <class Iter_t>
struct range
{
  Iter_t first, last;
  //
  //------------------------------------------------------------------------
  //  function : range
  /// @brief  empty constructor
  //------------------------------------------------------------------------
  range(void) { };
  //
  //------------------------------------------------------------------------
  //  function : range
  /// @brief  constructor with two parameters
  /// @param frs : iterator to the first element
  /// @param lst : iterator to the last element
  //-----------------------------------------------------------------------
  range(const Iter_t &frs, const Iter_t &lst): first(frs), last(lst) { };
  //
  //-----------------------------------------------------------------------
  //  function : empty
  /// @brief indicate if the range is empty
  /// @return  true : empty false : not empty
  //-----------------------------------------------------------------------
  bool empty(void) const { return (first == last); };
  //
  //-----------------------------------------------------------------------
  //  function : not_empty
  /// @brief indicate if the range is not empty
  /// @return  true : not empty false : empty
  //-----------------------------------------------------------------------
  bool not_empty(void) const {return (first != last); };
  //
  //-----------------------------------------------------------------------
  //  function : valid
  /// @brief  Indicate if the range is well constructed, and valid
  /// @return true : valid,  false : not valid
  //-----------------------------------------------------------------------
  bool valid(void) const { return ((last - first) >= 0); };
  //
  //-----------------------------------------------------------------------
  //  function : size
  /// @brief  return the size of the range
  /// @return size
  //-----------------------------------------------------------------------
  size_t size(void) const { return (last - first); };
  //
  //------------------------------------------------------------------------
  //  function : front
  /// @brief return an iterator to the first element of the range
  /// @return iterator
  //-----------------------------------------------------------------------
  Iter_t front(void) const { return first; };
  //
  //-------------------------------------------------------------------------
  //  function : back
  /// @brief return an iterator to the last element of the range
  /// @return iterator
  //-------------------------------------------------------------------------
  Iter_t back(void) const {return (last - 1); };
};

template<class Iter_t>
inline range<Iter_t> concat(const range<Iter_t> &it1, const range<Iter_t> &it2)
{
  return range<Iter_t>(it1.first, it2.last);
}

template<typename iterator_t>
struct bitonic_sort_operation
{
  iterator_t begin;
  iterator_t last;
};

#endif //TEST_SORT_BITONIC_HPP
