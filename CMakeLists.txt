cmake_minimum_required(VERSION 4.0)
project(particle_distance)

set(CMAKE_CXX_STANDARD 20)

set(SOURCE_FILES
        main.cpp
        geometry.cpp
        geometry.h
        data_io.cpp
        data_io.h
)

add_executable(particle_sim ${SOURCE_FILES})

if (APPLE)
    set(OMP_INCLUDE_DIR "/opt/homebrew/opt/libomp/include")
    set(OMP_LIB_DIR "/opt/homebrew/opt/libomp/lib")

    target_include_directories(particle_sim PUBLIC ${OMP_INCLUDE_DIR})
    target_compile_options(particle_sim PUBLIC -Xpreprocessor -fopenmp)
    target_link_directories(particle_sim PUBLIC ${OMP_LIB_DIR})
    target_link_libraries(particle_sim PUBLIC omp)

    message(STATUS "Mac OS detected. Using OpenMP from: ${OPEN_INCLUDE_DIR}")

else()
    find_package(OpenMP REQUIRED)
    target_link_libraries(particle_sim PUBLIC OPenMP::OpenMP_CXX)
endif()