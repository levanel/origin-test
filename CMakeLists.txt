cmake_minimum_required(VERSION 3.10)
project(ORIGIN VERSION 1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Threads REQUIRED)
include_directories(include)

add_library(core_engine src/syncengine.cpp)
target_link_libraries(core_engine PRIVATE Threads::Threads)

add_executable(run_phase1_test test/test_phase1_sync.cpp)
target_link_libraries(run_phase1_test PRIVATE core_engine Threads::Threads)

add_executable(run_phase3_mutex_fail_test test/test_phase3_mutex_fail.cpp)
target_link_libraries(run_phase3_mutex_fail_test PRIVATE core_engine Threads::Threads)


add_executable(run_phase3_pass_test test/test_phase3_pass.cpp)
target_link_libraries(run_phase3_pass_test PRIVATE core_engine Threads::Threads)

#test_phase3_pass