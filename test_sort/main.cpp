#include <iostream>
#include <string>
#include <vector>

#include "sorter.hpp"

int main() {
  std::vector<std::string> in_data = {"asdfqw1234325", "af3t t9unj9-wn=0vmerwg", "12213", "dfjhgkr", "mhioertphnj SGSdgadg"};
  //string_list in_data = {5,23,39,6,14,13,1,6,7,4,8,5,8,9,3,5,23};
  //string_list in_data = {};
  //std::list<int> in_data = {5,60,23,6,14,39,13,1,49,6,7,4,10,8,5,8,9,3,5,23};
  //string_list in_data = {5,23,39,6,14,13,1,6,7,4,8,5,8,9,3,5,23};
  //string_list in_data = {30,23,39};
  
  sort_data_functor sort(in_data.begin(), in_data.end());
  sort();
  
  return 0;
}