cmake_minimum_required(VERSION 3.15)
project(MarkdownConverter VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(
    -Wall
    -Wextra
    -Werror
    -Wpedantic
    -fsanitize=undefined
)

add_link_options(-fsanitize=undefined)

# Find Conan dependencies
find_package(pybind11 REQUIRED)
find_package(Catch2 REQUIRED)

# Find system-installed cmark-gfm (via apt)
find_path(CMARK_GFM_INCLUDE_DIR cmark-gfm.h
    PATHS /usr/include /usr/local/include
)
find_library(CMARK_GFM_LIB NAMES cmark-gfm
    PATHS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu
)
find_library(CMARK_GFM_EXT_LIB NAMES cmark-gfm-extensions
    PATHS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu
)

if(NOT CMARK_GFM_INCLUDE_DIR OR NOT CMARK_GFM_LIB OR NOT CMARK_GFM_EXT_LIB)
    message(FATAL_ERROR "cmark-gfm not found. Run: sudo apt install libcmark-gfm-dev libcmark-gfm-extensions-dev")
endif()

# Create an imported target to mimic the old cmark-gfm::cmark-gfm interface
add_library(cmark-gfm::cmark-gfm UNKNOWN IMPORTED)
set_target_properties(cmark-gfm::cmark-gfm PROPERTIES
    IMPORTED_LOCATION "${CMARK_GFM_LIB}"
    INTERFACE_INCLUDE_DIRECTORIES "${CMARK_GFM_INCLUDE_DIR}"
)

add_library(cmark-gfm::cmark-gfm-extensions UNKNOWN IMPORTED)
set_target_properties(cmark-gfm::cmark-gfm-extensions PROPERTIES
    IMPORTED_LOCATION "${CMARK_GFM_EXT_LIB}"
    INTERFACE_INCLUDE_DIRECTORIES "${CMARK_GFM_INCLUDE_DIR}"
)

# Include directories
include_directories(include)

# Core Library
add_library(core_logic
    src/parser.cpp
    src/writers/latex_writer.cpp
    src/writers/typst_writer.cpp
    src/dispatcher.cpp
)
target_link_libraries(core_logic PRIVATE
    cmark-gfm::cmark-gfm
    cmark-gfm::cmark-gfm-extensions
)

# Pybind11 Module
pybind11_add_module(converter_bindings bindings/bindings.cpp)
target_link_libraries(converter_bindings PRIVATE core_logic)

# C++ Tests
add_executable(cpp_tests tests/cpp/test_main.cpp)
target_link_libraries(cpp_tests PRIVATE core_logic Catch2::Catch2WithMain)