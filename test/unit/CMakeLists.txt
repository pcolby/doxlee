# SPDX-FileCopyrightText: 2021-2024 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)

function(add_doxlee_unit_test name)
  add_executable(test${name} ${ARGN})

  target_include_directories(test${name} PRIVATE ${CMAKE_SOURCE_DIR}/src)

  target_link_libraries(
    test${name}
    PRIVATE libDoxlee
    PRIVATE ${TEXT_TEMPLATE_TARGET}
    PRIVATE Qt${QT_VERSION_MAJOR}::Test)

if(${QT_VERSION} VERSION_LESS "5.12")
  add_test(NAME ${name} COMMAND test${name})
else() # Support for TAP format added to Qt Test in 5.12.
  add_test(NAME ${name} COMMAND test${name} -o ${name}.tap,tap -o -,txt)
endif()
  set_tests_properties(${name} PROPERTIES LABELS "unit")
endfunction()

add_doxlee_unit_test(
  Doxml
  testdoxml.cpp
  testdoxml.h)

add_doxlee_unit_test(
  Renderer
  testrenderer.cpp
  testrenderer.h)
