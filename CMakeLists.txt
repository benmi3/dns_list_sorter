cmake_minimum_required(VERSION 3.27)
project(dns_list_sorter)

set(CMAKE_CXX_STANDARD 17)

# Find the libcurl library
find_package(CURL REQUIRED)

# Find the tomlplusplus library
find_package(tomlplusplus REQUIRED)

# Add the source file to the project
add_executable(dns_list_sorter
        src/dns_list_sorter.cpp
        src/dns_list_sorter.cpp)

# Link against the libcurl library
target_link_libraries(dns_list_sorter PRIVATE CURL::libcurl)