set(TEST_RESOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/res")
set(TEST_TEMP_DIR "${CMAKE_CURRENT_BINARY_DIR}")

function(tini_add_test name)
  add_executable(${name} ${ARGN})
  add_test(NAME ${name} COMMAND $<TARGET_FILE:${name}>)
  target_link_libraries(${name} PRIVATE 
    tini::tini 
    cunit::cunit
  )
  target_compile_definitions(${name} PRIVATE 
    TEST_RESOURCE_DIR="${TEST_RESOURCE_DIR}"
    TEST_TEMP_DIR="${TEST_TEMP_DIR}"
  )
endfunction()

tini_add_test(test_lifecycle test_lifecycle.c)
tini_add_test(test_section_api test_section_api.c)
tini_add_test(test_key_api test_key_api.c)
tini_add_test(test_value_getter test_value_getter.c)
tini_add_test(test_parser test_parser.c)
tini_add_test(test_serializer test_serializer.c)
tini_add_test(test_edge_cases test_edge_cases.c)
tini_add_test(test_error_handling test_error_handling.c)
tini_add_test(test_iterator test_iterator.c)
tini_add_test(test_cpp test_cpp.cpp)
