add_executable(cotton_int src/cotton.cpp)

add_compile_definitions("DEFAULT_SOURCE_FILENAME=\"/home/lis05/Projects/Cotton/tests/program.ctn\"")

target_link_libraries(cotton_int PRIVATE cotton_lib)