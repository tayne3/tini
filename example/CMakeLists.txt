string(TIMESTAMP TINI_BUILD_DATE "%Y-%m-%d")

add_executable(tini_sample sample.c)
target_link_libraries(tini_sample PRIVATE
  tini_internal_options
  tini::tini
)

add_executable(tini_sample_cpp sample_cpp.cpp)
target_link_libraries(tini_sample_cpp PRIVATE
  tini_internal_options
  tini::tini
)

add_executable(tinix tinix.c)
target_link_libraries(tinix PRIVATE
  tini_internal_options
  tini::tini
)
target_compile_definitions(tinix PRIVATE
  -DTINI_COMPILER_ID="${CMAKE_C_COMPILER_ID}"
  -DTINI_COMPILER_VERSION="${CMAKE_C_COMPILER_VERSION}"
  -DTINI_VERSION="${PROJECT_VERSION}"
  -DTINI_BUILD_DATE="${TINI_BUILD_DATE}"
)
