#include <iostream>
#include <string>
#include <vector>
#include <list>

#include <random>

#include <boost/sort/sort.hpp>

#include "sort.hpp"

int main() {
  //std::vector<std::string> in_data = {"asdfqw1234325", "af3t t9unj9-wn=0vmerwg", "12213", "dfjhgkr", "mhioertphnj SGSdgadg"};
  
  
  /*std::vector<int> in_data1(0x0fffffff);
  for(int i = 0; i < in_data1.size(); ++i)
  {
    in_data1[i] = dist(rd);
  }*/
  
//std::vector<int> in_data1(0x0fffffff);
  std::vector<int> in_data1(0x08000000);
  std::vector<int> in_data2(0x08000000);
  std::vector<int> in_data3(0x08000000);
  
  auto block_size = in_data1.size()/8;
  size_t current_ind = 0;
  std::vector<std::future<void>> results;
  std::random_device rd;
  std::uniform_int_distribution<uint64_t> dist(0, (uint32_t)-1);
  
  for(int i = 0; i < in_data1.size(); ++i)
  {
      /*  static int x = 0;
        ++x;
        if(x%4 == 0)
          tmp = dist(rd);*/
        in_data1[i] = dist(rd);
   }
   
  std::copy(in_data1.begin(), in_data1.end(), in_data2.begin());
  std::copy(in_data1.begin(), in_data1.end(), in_data3.begin());
 /*
  std::vector<int> in_data2 = {
   1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32
  };
 */
 // auto& thread_pool = thread_pool_t::instance();
  
  {
    auto start = std::chrono::system_clock::now();
    //parallel_merge_sort(in_data1.begin(), in_data1.end());
    std::sort(in_data1.begin(), in_data1.end());
    //boost::sort::block_indirect_sort(in_data1.begin(), in_data1.end());
    auto end = std::chrono::system_clock::now();
  
    std::chrono::milliseconds diff = std::chrono::duration_cast<std::chrono::milliseconds> (end - start);
    std::cout << "STD Sort time = " << diff.count() << " ms\n";
  }
  
  {
    auto start = std::chrono::system_clock::now();
    parallel_merge_sort(in_data2.begin(), in_data2.end());
    //boost::sort::block_indirect_sort(in_data1.begin(), in_data1.end());
    auto end = std::chrono::system_clock::now();
    
    std::chrono::milliseconds diff = std::chrono::duration_cast<std::chrono::milliseconds> (end - start);
    std::cout << "Parallel Sort time = " << diff.count() << " ms\n";
  }
  {
    auto start = std::chrono::system_clock::now();
    boost::sort::block_indirect_sort(in_data3.begin(), in_data3.end());
    auto end = std::chrono::system_clock::now();
    
    std::chrono::milliseconds diff = std::chrono::duration_cast<std::chrono::milliseconds> (end - start);
    std::cout << "Boost Sort time = " << diff.count() << " ms\n";
  }
  
  //boost::sort::block_indirect_sort(in_data1.begin(), in_data1.end());
  //string_list in_data = {5,23,39,6,14,13,1,6,7,4,8,5,8,9,3,5,23};
  //string_list in_data = {};
  //string_list in_data = {5,23,39,6,14,13,1,6,7,4,8,5,8,9,3,5,23};
  //string_list in_data = {30,23,39};
  
  bool res = std::equal(in_data1.begin(), in_data1.end(), in_data2.begin());
  if(res)
    std::cout << "All OK" << std::endl;
  else
    std::cout << "FAIL" << std::endl;
  return 0;
}
