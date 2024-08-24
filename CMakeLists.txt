cmake_minimum_required(VERSION 3.0.0)
project(Cotton VERSION 0.1.0 LANGUAGES C CXX)

#add_compile_definitions("COTTON_ENABLE_PROFILER")

# enable the line below before building Cotton for programming
# add_compile_options("-O3" "-march=native")

add_compile_options("-g")
add_compile_options("-std=c++20")


add_subdirectory(cotton_lib)
add_subdirectory(cotton_int)
add_subdirectory(cotton_modules)
