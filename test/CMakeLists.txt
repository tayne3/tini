file(GLOB_RECURSE SRCS_G CONFIGURE_DEPENDS "cases/*.cpp")
foreach(SRC ${SRCS_G})
  get_filename_component(TEST_NAME ${SRC} NAME_WE)
  add_executable(tini_${TEST_NAME} ${SRC} main.cpp)
  add_test(NAME tini_${TEST_NAME} COMMAND tini_${TEST_NAME})
  target_link_libraries(tini_${TEST_NAME} PRIVATE
    tini_internal_options
    tini::tini
  )
  target_include_directories(tini_${TEST_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/doctest
    ${CMAKE_CURRENT_SOURCE_DIR}/cases
  )
  target_compile_definitions(tini_${TEST_NAME} PRIVATE
    TEST_RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/res"
    TEST_TEMP_DIR="${CMAKE_CURRENT_BINARY_DIR}"
  )
endforeach()
