cmake_minimum_required(VERSION 4.0)
set(CMAKE_OSX_DEPLOYMENT_TARGET 26.5)
project(project2)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-fexperimental-library)
add_link_options(-fexperimental-library)

add_executable(project2
        main.cpp
        data.cpp
        experiments.cpp)