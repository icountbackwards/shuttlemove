cmake_minimum_required(VERSION 3.20)

project(Badminton LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Don't build GLFW extras
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)

# GLFW
add_subdirectory(external/glfw)

# GLAD
add_library(glad
    external/glad/src/glad.c
)

target_include_directories(glad PUBLIC
    external/glad/include
)

# Executable
add_executable(shuttlemove
    src/main.cpp src/renderer.cpp src/mesh.cpp src/gamestate.cpp
)

# Include GLM headers
target_include_directories(BadmintonSim PRIVATE
    ${CMAKE_SOURCE_DIR}
    external/glm
    external/stb_image
    external/tinyobjloader
)

target_link_libraries(BadmintonSim PRIVATE
    glad
    glfw
)
