#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>

#include <random>

#include <boost/sort/sort.hpp>

#include "sort.hpp"

#include "json.hpp"

int main() {
  std::ifstream fin("test_string_data.json");
  nlohmann::json in_data;
  fin >> in_data;
  using string_data_set = std::vector<std::string>;
  std::vector<string_data_set> in_sets;
  assert(in_data.is_array());
  for(auto& set: in_data)
  {
    in_sets.push_back(set.get<string_data_set>());
  }
  
  for(auto& in_set: in_sets)
  {
    string_data_set copy2(in_set.size()), copy3(in_set.size());
    std::copy(in_set.begin(), in_set.end(), copy2.begin());
    std::copy(in_set.begin(), in_set.end(), copy3.begin());
    
    {
      auto start = std::chrono::system_clock::now();
      parallel_merge_sort(in_set.begin(), in_set.end());
      //boost::sort::block_indirect_sort(in_data1.begin(), in_data1.end());
      auto end = std::chrono::system_clock::now();
    
      std::chrono::milliseconds diff = std::chrono::duration_cast<std::chrono::milliseconds> (end - start);
      std::cout << "parallel sort time = " << diff.count() << " ms\n";
    }
    {
      auto start = std::chrono::system_clock::now();
      //parallel_merge_sort(in_data1.begin(), in_data1.end());
      boost::sort::pdqsort(copy2.begin(), copy2.end());
      //boost::sort::block_indirect_sort(in_data1.begin(), in_data1.end());
      auto end = std::chrono::system_clock::now();
    
      std::chrono::milliseconds diff = std::chrono::duration_cast<std::chrono::milliseconds> (end - start);
      std::cout << "std sort time = " << diff.count() << " ms\n";
    }
    {
      auto start = std::chrono::system_clock::now();
      boost::sort::block_indirect_sort(copy3.begin(), copy3.end());
      auto end = std::chrono::system_clock::now();
    
      std::chrono::milliseconds diff = std::chrono::duration_cast<std::chrono::milliseconds> (end - start);
      std::cout << "boost Sort time = " << diff.count() << " ms\n";
    }
    bool res = std::equal(in_set.begin(), in_set.end(), copy2.begin());
    if(res)
      std::cout << "All OK" << std::endl;
    else
      std::cout << "FAIL" << std::endl;
  }
  std::ofstream fout("string_data_out.json");
  nlohmann::json j_out;
  for(auto& in_set: in_sets)
  {
    j_out.push_back(in_set);
  }
  fout << j_out;
  
  return 0;
}
