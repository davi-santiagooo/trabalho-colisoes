cmake_minimum_required(VERSION 4.3)
project(colisoes)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.8
)

FetchContent_MakeAvailable(SDL3)

add_subdirectory(src)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
