cmake_minimum_required(VERSION 3.10)
project(test_sort)

SET(BOOST_COMPONENTS)
LIST(APPEND BOOST_COMPONENTS log date_time system filesystem program_options serialization chrono unit_test_framework locale thread exception iostreams)
SET( Boost_USE_STATIC_LIBS ON CACHE STRING "ON or OFF" )

SET(BOOST_ROOT $ENV{BOOST_ROOT})
set(Boost_USE_MULTITHREADED ON)
set(BOOST_ALL_DYN_LINK OFF)

FIND_PACKAGE(Boost 1.65 REQUIRED COMPONENTS ${BOOST_COMPONENTS})
SET(Boost_LIBRARIES ${Boost_LIBRARIES})

include_directories(${Boost_INCLUDE_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -stdlib=libc++" )

add_executable(test_sort main.cpp range.hpp sorter.cpp sorter.hpp thread_pool.hpp)

target_link_libraries(test_sort ${Boost_LIBRARIES} pthread)
