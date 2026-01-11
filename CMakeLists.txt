cmake_minimum_required(VERSION 3.14)

include(${CMAKE_CURRENT_LIST_DIR}/cmake/PreventInSourceBuilds.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/GitVersion.cmake)
git_version_info(VERSION TINI_VERSION DEFAULT_VERSION 0.1.0)
project(tini VERSION ${TINI_VERSION})

include(${CMAKE_CURRENT_LIST_DIR}/cmake/ProjectIsTopLevel.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/OptionVariables.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/ProjectConfig.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/CPM.cmake)

if(TINI_BUILD_SHARED)
  add_library(tini_shared SHARED)
  set(TINI_TARGET tini_shared)
  message(STATUS "tini v${TINI_VERSION} shared library")
else()
  add_library(tini_static STATIC)
  set(TINI_TARGET tini_static)
  message(STATUS "tini v${TINI_VERSION} static library")
endif()
add_library(tini::tini ALIAS ${TINI_TARGET})

target_sources(${TINI_TARGET} PRIVATE 
  src/core.c
  src/section.c
  src/key.c
  src/utils.c
)
target_link_libraries(${TINI_TARGET} PUBLIC tini_compile_dependency)

if(TINI_BUILD_TEST)
  CPMAddPackage(
    NAME cunit
    URL "https://codeload.github.com/tayne3/cunit/tar.gz/refs/tags/v0.2.4"
  )
  enable_testing()
  add_subdirectory(test)
endif()

if(TINI_BUILD_EXAMPLE)
  add_subdirectory(example)
endif()
