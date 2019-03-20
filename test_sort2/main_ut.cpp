#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <type_traits>

#include <random>

#define BOOST_TEST_MODULE main_ut test

#include <boost/sort/sort.hpp>
#include <boost/test/unit_test.hpp>

#include "sort.hpp"

#include "json.hpp"

template <typename T>
void ut_main(T& in_sets, nlohmann::json& in_data)
{
  using data_set_t = typename T::value_type;
  for(auto& set: in_data)
  {
    in_sets.push_back(set.get<data_set_t>());
  }
  for(auto& in_set: in_sets)
  {
    BOOST_TEST_MESSAGE("=== in data set begin ===");
    data_set_t copy2(in_set.size()), copy3(in_set.size());
    std::copy(in_set.begin(), in_set.end(), copy2.begin());
    std::copy(in_set.begin(), in_set.end(), copy3.begin());
    
    {
      auto start = std::chrono::system_clock::now();
      parallel_merge_sort(in_set.begin(), in_set.end());
      auto end = std::chrono::system_clock::now();
      
      std::chrono::milliseconds diff = std::chrono::duration_cast<std::chrono::milliseconds> (end - start);
      BOOST_TEST_MESSAGE("custom parallel,           time = " << diff.count() << " ms");
    }
    {
      auto start = std::chrono::system_clock::now();
      std::sort(copy2.begin(), copy2.end());
      auto end = std::chrono::system_clock::now();
      
      std::chrono::milliseconds diff = std::chrono::duration_cast<std::chrono::milliseconds> (end - start);
      BOOST_TEST_MESSAGE("std sort,                  time = " << diff.count() << " ms");
    }
    {
      auto start = std::chrono::system_clock::now();
      boost::sort::block_indirect_sort(copy3.begin(), copy3.end());
      auto end = std::chrono::system_clock::now();
      
      std::chrono::milliseconds diff = std::chrono::duration_cast<std::chrono::milliseconds> (end - start);
      BOOST_TEST_MESSAGE("boost block indirect sort, time = " << diff.count() << " ms");
    }
    bool res = std::equal(in_set.begin(), in_set.end(), copy2.begin());
    BOOST_CHECK(res);
  }
}

BOOST_AUTO_TEST_CASE(int_sort)
{
  BOOST_TEST_MESSAGE("UT case: int sort ");
  nlohmann::json in_data;
  {
    std::ifstream fin("test_int_data.json");
    fin >> in_data;
  }
  assert(in_data.is_array());
  using data_set = std::vector<int>;
  std::vector<data_set> in_sets;
  ut_main(in_sets, in_data);
  std::ofstream fout("int_data_out.json");
  nlohmann::json j_out;
  for(auto& in_set: in_sets)
  {
    j_out.push_back(in_set);
  }
  fout << j_out;
}


BOOST_AUTO_TEST_CASE(string_sort)
{
  BOOST_TEST_MESSAGE("UT case: string sort ");
  nlohmann::json in_data;
  {
    std::ifstream fin("test_string_data.json");
    fin >> in_data;
  }
  assert(in_data.is_array());
  using data_set = std::vector<std::string>;
  std::vector<data_set> in_sets;
  ut_main(in_sets, in_data);
  std::ofstream fout("string_data_out.json");
  nlohmann::json j_out;
  for(auto& in_set: in_sets)
  {
    j_out.push_back(in_set);
  }
  fout << j_out;
}
