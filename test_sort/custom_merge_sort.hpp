//
// Created by arrayio on 3/20/19.
//

#ifndef TEST_SORT_CUSTOM_MERGE_SORT_HPP
#define TEST_SORT_CUSTOM_MERGE_SORT_HPP

#include <vector>
#include "range.hpp"

using boost::sort::common::range;

template <typename Iterator>
void merge_sort(Iterator begin, Iterator last) {
  size_t len = std::distance(begin, last);
  using value_type = typename Iterator::value_type;
  using range_t = range<Iterator>;
  std::vector<range_t> ranges;
  ranges.reserve(count_ranges(len));
  for(auto it = begin; it < last; ++it)
  {
    ranges.push_back(range_t(it,std::min((it+1), last)));
  }
  for(; ranges.size() > 1; )
  {
    size_t count = 0;
    std::vector<value_type> buf(ranges.begin()->size()*2);
    for(auto it = ranges.begin(); it < ranges.end(); ++count)
    {
      range_t buf_range(buf.begin(), buf.end());
      ranges[count] = (it + 1 != ranges.end())? merge_contiguous(*it,*(it+1), buf_range, std::less<value_type>()): *it;
      it += 2;
    }
    ranges.resize(count);
  }
}

#endif //TEST_SORT_CUSTOM_MERGE_SORT_HPP
