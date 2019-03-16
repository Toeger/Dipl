cmake_minimum_required(VERSION 3.1)

enable_language(CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

project(TSSTP)
add_executable(${PROJECT_NAME} main.cpp tags.cpp TSSTP.cpp thread_pool.cpp)
target_link_libraries(${PROJECT_NAME} Threads::Threads)

add_executable(${PROJECT_NAME}_TESTS tags.cpp TSSTP.cpp thread_pool.cpp test.cpp test_thread_private.cpp)
target_link_libraries(${PROJECT_NAME}_TESTS Threads::Threads)
