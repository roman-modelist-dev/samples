//
// Created by arrayio on 3/17/19.
//

#include "sort.hpp"

size_t step_count(size_t length)
{
  if(length == 0)
    return 0;
  size_t count = 64;
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

size_t count_merges(size_t length)
{
  constexpr int even_mask = 0x01;
  if (even_mask & length)
    return length/2+1;
  else
    return length/2;
}

