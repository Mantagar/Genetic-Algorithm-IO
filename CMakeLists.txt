cmake_minimum_required(VERSION 2.8)
project(Genetic_Algorithm_IO)

if (CMAKE_VERSION VERSION_LESS "3.1")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
else ()
    set (CMAKE_CXX_STANDARD 14)
endif ()

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

SET(CMAKE_C_COMPILER mpicc)
SET(CMAKE_CXX_COMPILER mpicxx)

set(SOURCES
        src/optim_functions.cpp
        src/main.cpp
        src/Island.cpp
        src/utils.cpp
        src/Problem.cpp
        src/ProblemFactory.cpp
        )

set(HEADERS
        src/optim_functions.h
        src/utils.h
        src/Island.h
        src/ProblemName.h
        src/Problem.h
        src/ProblemFactory.h
        )

add_executable(Genetic_Algorithm_IO ${SOURCES} ${HEADERS})

set(HEADER_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}
        src
        )

target_include_directories(Genetic_Algorithm_IO PRIVATE ${HEADER_DIRECTORIES})