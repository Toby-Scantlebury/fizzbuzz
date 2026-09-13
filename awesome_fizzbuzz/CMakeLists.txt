cmake_minimum_required(VERSION 3.11)

project(awesome_fizzbuzz CXX)

set(CXX_STANDARD 23)
set(CXX_STANDARD_REQUIRED ON)

# include(libcoro.cmake)

include(FetchContent)
FetchContent_Declare(
    libcoro
    GIT_REPOSITORY https://github.com/jbaldwin/libcoro.git
    GIT_TAG        v0.16.0
)
FetchContent_MakeAvailable(libcoro)

add_executable(${PROJECT_NAME} awesome_fizzbuzz.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE libcoro)
