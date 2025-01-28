set (CMAKE_CXX_STANDARD 11)
project(PackageTest CXX)
cmake_minimum_required(VERSION 3.15)

# Include the Conan-generated files
find_package(xmsextractor CONFIG REQUIRED)

add_executable(example example.cpp)

# Link Conan dependencies
target_link_libraries(example)
