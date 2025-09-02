cmake_minimum_required(VERSION 3.10)
project(ThreadPoolProject)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compile options for threading
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Add executable
add_executable(threadpool main.cpp)

# Link threads
target_link_libraries(threadpool Threads::Threads)