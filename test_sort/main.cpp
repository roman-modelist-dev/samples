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
  nlohmann::json in_jdata;
  std::cin >> in_jdata;
  assert(in_jdata.is_array());
  if(in_jdata.empty())
  {
    std::cout << "[]" << std::endl;
    return 0;
  }
  using string_data_set = std::vector<std::string>;
  auto in_data = in_jdata.get<string_data_set>();
  
  parallel_merge_sort(in_data.begin(), in_data.end());
  
  nlohmann::json j_out(in_data);
  std::cout << j_out << std::endl;
  
  return 0;
}
