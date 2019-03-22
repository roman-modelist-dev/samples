cmake_minimum_required(VERSION 3.10)
project(test_sort)

SET(BOOST_COMPONENTS)
LIST(APPEND BOOST_COMPONENTS unit_test_framework)
SET( Boost_USE_STATIC_LIBS ON CACHE STRING "ON or OFF" )

SET(BOOST_ROOT $ENV{BOOST_ROOT})
set(Boost_USE_MULTITHREADED ON)
set(BOOST_ALL_DYN_LINK OFF)

FIND_PACKAGE(Boost 1.65 REQUIRED COMPONENTS ${BOOST_COMPONENTS})
SET(Boost_LIBRARIES ${Boost_LIBRARIES})

include_directories(${Boost_INCLUDE_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++" )


file(GLOB FILES_SRC "./sort.cpp" "./*.hpp" )

add_executable(test_sort main.cpp ${FILES_SRC})
add_executable(unit_test main_ut.cpp ${FILES_SRC})

target_link_libraries(test_sort ${Boost_LIBRARIES} pthread)
target_link_libraries(unit_test ${Boost_LIBRARIES} pthread)
