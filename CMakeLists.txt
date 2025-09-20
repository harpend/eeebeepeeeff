cmake_minimum_required(VERSION 3.15)
project(eeebeepeeeff LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include)
find_package(PkgConfig REQUIRED)
pkg_check_modules(CAPSTONE REQUIRED capstone)
include_directories(${CAPSTONE_INCLUDE_DIRS})
link_directories(${CAPSTONE_LIBRARY_DIRS})
add_executable(eeebeepeeeff src/main.cpp src/loader.cpp src/disassembler.cpp)
target_link_libraries(eeebeepeeeff ${CAPSTONE_LIBRARIES})
