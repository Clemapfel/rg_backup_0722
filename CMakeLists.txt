cmake_minimum_required(VERSION 3.22)
project(mousetrap VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_STANDARD 20)

### Dependencies

find_library(OpenGL REQUIRED NAMES GL)
find_library(GLEW REQUIRED NAMES GLEW)

find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-3.0)

include(CheckIncludeFileCXX)
CHECK_INCLUDE_FILE_CXX("glm/glm.hpp" GLM_FOUND)
if(NOT GLM_FOUND)
    message(FATAL_ERROR "Missing Dependency: glm")
endif()

### Library

add_library(mousetrap SHARED mousetrap.hpp
    include/vector.hpp
    include/colors.hpp
    include/gl_canvas.hpp
    include/gl_common.hpp
        include/shader.hpp)

set_target_properties(mousetrap PROPERTIES
    LINKER_LANGUAGE CXX
    CXX_STANDARD 20
)
target_include_directories(mousetrap PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${GTKMM_INCLUDE_DIRS}
)
target_link_libraries(mousetrap PUBLIC
    ${OpenGL}
    ${GLEW}
    ${GLU}
    ${GTKMM_LIBRARIES}
)

target_link_directories(mousetrap PUBLIC
    ${GTKMM_LIBRARY_DIRS}
)

### Tests

add_executable(debug main.cpp)
target_link_libraries(debug mousetrap)
