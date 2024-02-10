cmake_minimum_required(VERSION 3.27)
project(cpap C)

set(CMAKE_C_STANDARD 11)
find_package(MPI REQUIRED)
add_executable(cpap main.c
        main_parallelo.c)
target_link_libraries(cpap PRIVATE MPI::MPI_C)
