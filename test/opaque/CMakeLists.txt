# SPDX-FileCopyrightText: 2021-2024 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

# Tip: To keep the temporary output directory for further inspection, you can skip the fixture cleanup steps like:
# ctest -FC '.*' -R <test-name-regex>

function(add_doxlee_opaque_test)
  # Parse and validate the input directories.
  cmake_parse_arguments(PARSE_ARGV 0 OPAQUE_TEST "" "NAME;DOXML_DIR;TEMPLATES_DIR;RENDERS_DIR" "")
  if(DEFINED OPAQUE_TEST_UNPARSED_ARGUMENTS)
    message(SEND_ERROR "Opaque test unparsed arguments: ${OPAQUE_TEST_UNPARSED_ARGUMENTS}")
  endif()
  if(DEFINED OPAQUE_TEST_KEYWORDS_MISSING_VALUES)
    message(SEND_ERROR "Opaque test keywords missing values: ${OPAQUE_TEST_KEYWORDS_MISSING_VALUES}")
  endif()
  if(NOT IS_DIRECTORY ${OPAQUE_TEST_DOXML_DIR})
    message(SEND_ERROR "Opaque test doxml path is not a directory: ${OPAQUE_TEST_DOXML_DIR}")
  endif()
  if(NOT IS_DIRECTORY ${OPAQUE_TEST_TEMPLATES_DIR})
    message(SEND_ERROR "Opaque test templates path is not a directory: ${OPAQUE_TEST_TEMPLATES_DIR}")
  endif()
  if(NOT IS_DIRECTORY ${OPAQUE_TEST_RENDERS_DIR})
    message(SEND_ERROR "Opaque test renders path is not a directory: ${OPAQUE_TEST_RENDERS_DIR}")
  endif()

  # Determine the test's temporary output directory.
  set(outputDir "${CMAKE_CURRENT_BINARY_DIR}/output/${OPAQUE_TEST_NAME}")
  message(DEBUG "Opaque test will use output directory: ${outputDir}")
  if(EXISTS ${outputDir})
    message(WARNING "Opaque test output path already exists: ${outputDir}")
  endif()

  # Setup the test, along with its fixtures.
  add_test(NAME ${OPAQUE_TEST_NAME}MakeDir COMMAND ${CMAKE_COMMAND} -E make_directory   ${outputDir})
  add_test(NAME ${OPAQUE_TEST_NAME}Render  COMMAND doxlee -i ${OPAQUE_TEST_DOXML_DIR} -t ${OPAQUE_TEST_TEMPLATES_DIR} -o ${outputDir} -f)
  add_test(NAME ${OPAQUE_TEST_NAME}Compare COMMAND diff -r ${OPAQUE_TEST_RENDERS_DIR} ${outputDir})
  add_test(NAME ${OPAQUE_TEST_NAME}Cleanup COMMAND ${CMAKE_COMMAND} -E remove_directory ${outputDir})
  set_tests_properties(${OPAQUE_TEST_NAME}MakeDir PROPERTIES FIXTURES_SETUP    ${OPAQUE_TEST_NAME}OutputDir)
  set_tests_properties(${OPAQUE_TEST_NAME}Render  PROPERTIES FIXTURES_REQUIRED ${OPAQUE_TEST_NAME}OutputDir)
  set_tests_properties(${OPAQUE_TEST_NAME}Render  PROPERTIES FIXTURES_SETUP    ${OPAQUE_TEST_NAME}RenderOutput)
  set_tests_properties(${OPAQUE_TEST_NAME}Compare PROPERTIES FIXTURES_REQUIRED ${OPAQUE_TEST_NAME}RenderOutput)
  set_tests_properties(${OPAQUE_TEST_NAME}Cleanup PROPERTIES FIXTURES_CLEANUP  ${OPAQUE_TEST_NAME}RenderOutput)
  set_tests_properties(${OPAQUE_TEST_NAME}Compare PROPERTIES LABELS "opaque")

  # Enable re-creating the test's renders, for internal (Doxlee) development.
  add_custom_target(
    ${OPAQUE_TEST_NAME}Render
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${OPAQUE_TEST_RENDERS_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory   ${OPAQUE_TEST_RENDERS_DIR}
    COMMAND doxlee -i ${OPAQUE_TEST_DOXML_DIR} -t ${OPAQUE_TEST_TEMPLATES_DIR} -o ${OPAQUE_TEST_RENDERS_DIR} -f
    COMMENT "Recreating renders for test: ${OPAQUE_TEST_NAME}")
  add_dependencies(updateTestData ${OPAQUE_TEST_NAME}Render)
endfunction()

function(add_doxlee_opaque_tests dir)
  message(VERBOSE "Looking for opaque test data in: ${dir}")
  file(GLOB doxmlDirs     LIST_DIRECTORIES true RELATIVE ${dir}/doxml/     CONFIGURE_DEPENDS ${dir}/doxml/*)
  file(GLOB templatesDirs LIST_DIRECTORIES true RELATIVE ${dir}/templates/ CONFIGURE_DEPENDS ${dir}/templates/*)
  message(DEBUG "Found doxml directories: ${doxmlDirs}")
  message(DEBUG "Found templates directories: ${templatesDirs}")
  foreach(doxmlDir ${doxmlDirs})
    if(NOT IS_DIRECTORY ${dir}/doxml/${doxmlDir})
      continue()
    endif()
    foreach(templatesDir ${templatesDirs})
      if(NOT IS_DIRECTORY ${dir}/templates/${templatesDir})
        continue()
      endif()

      # Set testName to a CamelCase conversion of the templates and doxml directory names.
      string(REGEX MATCHALL "[a-zA-Z0-9]+" words "${templatesDir} ${doxmlDir}")
      set(testName "")
      foreach(word ${words})
        string(SUBSTRING ${word} 0 1 firstLetter)
        string(SUBSTRING ${word} 1 -1 remainder)
        string(TOUPPER ${firstLetter} firstLetter)
        string(TOLOWER ${remainder} remainder)
        string(APPEND testName ${firstLetter} ${remainder})
      endforeach()

      # Add a test for this combination of templates and doxml.
      add_doxlee_opaque_test(
        NAME ${testName}
        DOXML_DIR     ${dir}/doxml/${doxmlDir}
        TEMPLATES_DIR ${dir}/templates/${templatesDir}
        RENDERS_DIR   ${dir}/renders/${templatesDir}-${doxmlDir})
    endforeach()
  endforeach()
  message(VERBOSE "Finished looking for opaque test data in: ${dir}")
endfunction()

add_doxlee_opaque_tests(${CMAKE_SOURCE_DIR}/demo)
