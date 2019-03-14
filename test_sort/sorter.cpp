//
// Created by arrayio on 3/12/19.
//

#include "sorter.hpp"

uint8_t power2_more_than(size_t length)
{
  size_t power = step_count(length);
  return 1 << power;
}

uint8_t step_count(size_t length)
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
