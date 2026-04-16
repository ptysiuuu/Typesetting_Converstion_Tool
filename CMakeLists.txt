cmake_minimum_required(VERSION 3.20)
project(MarkdownConverter VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(project_warnings INTERFACE)
target_compile_options(project_warnings INTERFACE
    -Wall
    -Wextra
    -Werror
    -Wpedantic
    -fsanitize=undefined
)
target_link_options(project_warnings INTERFACE -fsanitize=undefined)

if(APPLE)
    target_compile_options(project_warnings INTERFACE -Wno-c2y-extensions)
endif()

if(POLICY CMP0148)
    cmake_policy(SET CMP0148 OLD)
endif()

# ── Conan dependencies ─────────────────────────────────────────────────────
find_package(pybind11 REQUIRED)
find_package(Catch2  REQUIRED)

# ── cmark-gfm ─────────────────────────────────────────────────────────────
include(FetchContent)

# CACHE BOOL is required to propagate into the subproject
set(CMARK_TESTS  OFF CACHE BOOL "" FORCE)
set(CMARK_SHARED OFF CACHE BOOL "" FORCE)
set(CMARK_STATIC ON  CACHE BOOL "" FORCE)

FetchContent_Declare(
    cmark_gfm
    GIT_REPOSITORY https://github.com/github/cmark-gfm.git
    GIT_TAG        0.29.0.gfm.13
)
FetchContent_MakeAvailable(cmark_gfm)

if(TARGET libcmark-gfm_static)
    set(CMARK_TARGET libcmark-gfm_static)
elseif(TARGET cmark-gfm)
    set(CMARK_TARGET cmark-gfm)
else()
    message(FATAL_ERROR "Could not find cmark-gfm target after FetchContent")
endif()

if(TARGET libcmark-gfm-extensions_static)
    set(CMARK_EXT_TARGET libcmark-gfm-extensions_static)
elseif(TARGET cmark-gfm-extensions)
    set(CMARK_EXT_TARGET cmark-gfm-extensions)
else()
    message(FATAL_ERROR "Could not find cmark-gfm-extensions target after FetchContent")
endif()

add_library(cmark-gfm::cmark-gfm            ALIAS ${CMARK_TARGET})
add_library(cmark-gfm::cmark-gfm-extensions ALIAS ${CMARK_EXT_TARGET})

# ── Core library ───────────────────────────────────────────────────────────
include_directories(include)

add_library(core_logic OBJECT
    src/parser.cpp
    src/writers/latex_writer.cpp
    src/writers/typst_writer.cpp
    src/dispatcher.cpp
)
target_link_libraries(core_logic PRIVATE
    cmark-gfm::cmark-gfm
    cmark-gfm::cmark-gfm-extensions
    project_warnings
)

# ── Python bindings ────────────────────────────────────────────────────────
pybind11_add_module(converter_bindings bindings/bindings.cpp)
target_link_libraries(converter_bindings PRIVATE
    core_logic
    project_warnings
)

# ── C++ tests ─────────────────────────────────────────────────────────────
add_executable(cpp_tests tests/cpp/test_main.cpp)
target_link_libraries(cpp_tests PRIVATE
    core_logic
    Catch2::Catch2WithMain
    project_warnings
)