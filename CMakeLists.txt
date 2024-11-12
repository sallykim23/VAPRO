cmake_minimum_required(VERSION 3.5)
project(Vapro)

find_package(OpenMP REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

# Required packages: libunwind, papi, jsoncpp

SET(CMAKE_C_COMPILER mpicc)
SET(CMAKE_CXX_COMPILER mpicxx)


SET(CMAKE_CXX_FLAGS "-Wall -g -O2 -Wno-deprecated-declarations -Wno-sign-compare")
SET(CMAKE_SHARED_LINKER_FLAGS "-g -ldl")
ADD_DEFINITIONS(-DDEBUG)
set(CMAKE_CXX_STANDARD 11)

# shared library
#set(WRAP ${PROJECT_SOURCE_DIR}/wrap/wrap.py)
#include(${PROJECT_SOURCE_DIR}/wrap/WrapConfig.cmake)
link_directories(${CMAKE_SOURCE_DIR}/jevents)

#add_wrapped_file(papicnt.cpp papicnt.w)
add_library(papicnt SHARED papi_wrap.h papi_wrap.cpp papicnt.cpp clustering.cpp clustering.h online_analyse.cpp online_analyse.h utility.cpp utility.h SocketClient.cpp SocketClient.h pmu.cpp pmu.h wrapper.cpp pmc_wrapper.h pmc.cpp)
target_link_libraries(papicnt PUBLIC unwind unwind-x86_64 papi jsoncpp jevents OpenMP::OpenMP_CXX )
# Microbenchmarks

add_executable(example_omp_program example_omp_program.cpp)
target_link_libraries(example_omp_program papicnt)



