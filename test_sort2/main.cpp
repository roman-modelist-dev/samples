#include <iostream>
#include <string>
#include <vector>
#include <list>

#include <random>

#include <boost/sort/sort.hpp>

#include "sort.hpp"

int main() {
  //std::vector<std::string> in_data = {"asdfqw1234325", "af3t t9unj9-wn=0vmerwg", "12213", "dfjhgkr", "mhioertphnj SGSdgadg"};
  
  std::random_device rd;
  std::uniform_int_distribution<uint64_t> dist(0, (uint32_t)-1);
  
  /*std::vector<int> in_data1(0x0fffffff);
  for(int i = 0; i < in_data1.size(); ++i)
  {
    in_data1[i] = dist(rd);
  }*/
  
 // std::vector<int> in_data1(0x0fffffff);
  std::vector<int> in_data1(0x000fffff);
  for(int i = 0; i < in_data1.size(); ++i)
  {
    in_data1[i] = dist(rd);
  }
  /*
  std::vector<int> in_data1 = {
   5, 60, 23, 6, 14, 39, 13, 1, 49, 6, 7, 4, 10, 8, 5, 8, 9, 3, 5, 23, 5, 60, 23, 6, 14, 39, 13, 1, 49, 6, 7, 4, 20, 8,
    5, 8, 9, 3, 5, 23, 5, 60, 23, 6, 14, 39, 13, 1, 49, 6, 7, 4, 10, 8, 5, 8, 9, 3, 5, 23, 5, 60, 23, 6, 14, 39, 13, 1,
   5, 60, 23, 6, 14, 39, 13, 1, 49, 6, 7, 4, 10, 8, 5, 8, 9, 3, 5, 23, 5, 60, 23, 6, 14, 39, 13, 1, 49, 6, 7, 4, 20, 8,
   5, 8, 9, 3, 5, 23, 5, 60, 23, 6, 14, 39, 13, 1, 49, 6, 7, 4, 10, 8, 5, 8, 9, 3, 5, 23, 5, 60, 23, 6, 14, 39, 13, 1,
   49, 6, 7, 4, 20, 8,
  };*/
  
 // auto& thread_pool = thread_pool_t::instance();
  
  auto start = std::chrono::system_clock::now();
  parallel_merge_sort(in_data1.begin(), in_data1.end());
  //std::sort(in_data1.begin(), in_data1.end());
  //boost::sort::block_indirect_sort(in_data1.begin(), in_data1.end());
  auto end = std::chrono::system_clock::now();
  
  std::chrono::milliseconds diff = std::chrono::duration_cast<std::chrono::milliseconds> (end - start);
  std::cout << "Sort time = " << diff.count() << " ms\n";
  //boost::sort::block_indirect_sort(in_data1.begin(), in_data1.end());
  //string_list in_data = {5,23,39,6,14,13,1,6,7,4,8,5,8,9,3,5,23};
  //string_list in_data = {};
  //string_list in_data = {5,23,39,6,14,13,1,6,7,4,8,5,8,9,3,5,23};
  //string_list in_data = {30,23,39};
  
  int d = 0;
  return 0;
}
