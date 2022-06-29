cmake_minimum_required(VERSION 3.22)
project(mousetrap VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_STANDARD 20)

### Dependencies

find_library(SDL2 REQUIRED NAMES SDL2)
find_library(SDL2_image REQUIRED NAMES SDL2_image)
find_library(SDL2_mixer REQUIRED NAMES SDL2_mixer)
find_library(OpenGL REQUIRED NAMES GL)
find_library(GLEW REQUIRED NAMES GLEW)

include(CheckIncludeFileCXX)
CHECK_INCLUDE_FILE_CXX("glm/glm.hpp" GLM_FOUND)
if(NOT GLM_FOUND)
    message(FATAL_ERROR "Missing Dependency: glm")
endif()

### Library

add_library(mousetrap SHARED
    include/keycodes.hpp
    include/input_handler.hpp
    .src/input_handler.inl
    include/time.hpp
    .src/time.inl
        include/window.hpp include/renderable.hpp include/transform.hpp include/render_target.hpp include/vector.hpp .src/window.inl include/image.hpp include/colors.hpp .src/colors.inl include/geometric_shapes.hpp include/angle.hpp .src/angle.inl include/shader.hpp include/common.hpp include/texture.hpp .src/texture.inl .src/transform.inl)
set_target_properties(mousetrap PROPERTIES
    LINKER_LANGUAGE CXX
    CXX_STANDARD 20
    #LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)
target_include_directories(mousetrap PUBLIC
    ${CMAKE_SOURCE_DIR}
)
target_link_libraries(mousetrap PUBLIC
    ${SDL2}
    ${SDL2_image}
    ${SDL2_mixer}
    ${OpenGL}
    ${GLEW}
)

### Tests

add_executable(debug main.cpp)
target_link_libraries(debug mousetrap)

