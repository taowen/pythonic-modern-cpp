cmake_minimum_required(VERSION 3.5)
project(string)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
add_subdirectory(_vendor/pcre2 ${PROJECT_BINARY_DIR}/pcre)
include_directories(_vendor/catch/include)
include_directories(_vendor/catch-ext/include)
include_directories(_vendor/pythonic/include)
include_directories(_vendor/range-v3/include)
include_directories(_vendor/fmt/include)
include_directories(_vendor/folly/include)
include_directories(_vendor/boost/include)
include_directories(${PROJECT_BINARY_DIR}/pcre)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -stdlib=libc++")
if (USE_MODULES)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -stdlib=libc++ -fno-autolink -fmodules -fmodules-cache-path=${CMAKE_SOURCE_DIR}/build")
endif(USE_MODULES)
set(SOURCE_FILES main.cpp)
add_executable(string ${SOURCE_FILES})
add_executable(pythonic
    _vendor/catch-ext/include/catch_ext.hpp
    _vendor/pythonic/include/pythonic/utf8.hpp
    _vendor/pythonic/include/pythonic/regexs.hpp)
target_link_libraries( string pcre2-8 )
