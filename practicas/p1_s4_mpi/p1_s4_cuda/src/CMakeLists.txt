project(ConcurrenciaMPI)

set(CMAKE_C_COMPILER "mpicc")
set(CMAKE_CXX_COMPILER "mpiCC")
set(CMAKE_VERBOSE_MAKEFILE "true")
set(CMAKE_INCLUDE_PATH "/usr/include")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 --with-cuda-libdir=DIR")

set(SOURCE_FILES
    debug_time.h
    matriz.cpp
    matriz.h
    main.cpp
    practica2Cuda.h
    practica2Cuda.cu
    slave.cpp
    slave.h)

add_executable(mpi ${SOURCE_FILES})