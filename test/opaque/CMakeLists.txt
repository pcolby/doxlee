add_test(
  NAME testOpaque
  COMMAND test.sh
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set_tests_properties(
  testOpaque
  PROPERTIES ENVIRONMENT DOXLEE=${PROJECT_BINARY_DIR}/src/doxlee)
