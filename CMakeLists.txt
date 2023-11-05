cmake_minimum_required(VERSION 3.12)

project(BackTest)
include_directories(hxx)
include_directories(hxx/spdlog)
aux_source_directory(cxx source)
set(CMAKE_BUILD_TYPE Release)
add_executable(BackTest ${source})
target_compile_features(BackTest PRIVATE cxx_std_20)