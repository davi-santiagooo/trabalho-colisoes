cmake_minimum_required(VERSION 4.1)
project(ColisoesGL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)

set(target_name ${PROJECT_NAME})
add_executable(${target_name})

# GLFW
add_subdirectory(vendors/glfw-3.4)

# OpenGL
find_package(OpenGL REQUIRED)

# GLAD
target_include_directories(${target_name} PRIVATE  vendors/glad/include)
target_sources(${target_name} PRIVATE vendors/glad/src/gl.c)

target_include_directories(
    ${target_name}
    PRIVATE
        include/ColisoesGL
        assets
)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")
target_sources(${target_name} PRIVATE ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    OpenGL::GL
    glfw
)