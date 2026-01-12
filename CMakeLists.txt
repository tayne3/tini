cmake_minimum_required(VERSION 3.14)

include(${CMAKE_CURRENT_LIST_DIR}/cmake/PreventInSourceBuilds.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/GitVersion.cmake)
git_version_info(VERSION TINI_VERSION DEFAULT_VERSION 0.1.1)
project(tini VERSION ${TINI_VERSION})

include(${CMAKE_CURRENT_LIST_DIR}/cmake/ProjectIsTopLevel.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/OptionVariables.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/ProjectConfig.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/CPM.cmake)

set(TINI_LIB_TYPE STATIC)
if(BUILD_SHARED_LIBS)
  set(TINI_LIB_TYPE SHARED)
endif()
message(STATUS "tini v${TINI_VERSION} ${TINI_LIB_TYPE} library")

add_library(tini ${TINI_LIB_TYPE})
add_library(tini::tini ALIAS tini)
set_target_properties(tini PROPERTIES
  VISIBILITY_INLINES_HIDDEN TRUE
  C_VISIBILITY_PRESET hidden
  CXX_VISIBILITY_PRESET hidden
)
target_link_libraries(tini PRIVATE tini_compile_dependency)
target_include_directories(tini PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
)
target_sources(tini PRIVATE 
  src/core.c
  src/section.c
  src/key.c
  src/utils.c
)

include(GenerateExportHeader)
generate_export_header(tini
  BASE_NAME tini
  EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/include/tini/tini_export.h
)
target_include_directories(tini PUBLIC
  ${CMAKE_CURRENT_BINARY_DIR}/include
)

if(TINI_BUILD_EXAMPLE)
  add_subdirectory(example)
endif()

if(TINI_BUILD_TEST)
  CPMAddPackage(
    NAME cunit
    URL "https://codeload.github.com/tayne3/cunit/tar.gz/refs/tags/v0.2.5"
    URL_HASH "SHA256=73429bf8bc733ce5431b012689e56f9d255bab3d6d1ac745311f81514b322186"
    OPTIONS "BUILD_SHARED_LIBS OFF"
  )
  enable_testing()
  add_subdirectory(test)
endif()
