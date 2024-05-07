# SPDX-FileCopyrightText: 2021-2024 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

add_test(
  NAME testOpaque
  COMMAND test.sh
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set_tests_properties(
  testOpaque
  PROPERTIES ENVIRONMENT DOXLEE=${PROJECT_BINARY_DIR}/src/doxlee)
