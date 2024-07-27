cmake_minimum_required(VERSION 3.0.0)
project(Cotton VERSION 0.1.0 LANGUAGES C CXX)

add_compile_options("-Wall" "-Wpedantic" "-Wextra" "-fexceptions" "-g")
#add_compile_options("-O3" "-march=native")


add_subdirectory(cotton_lib)
add_subdirectory(cotton_int)
