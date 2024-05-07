# SPDX-FileCopyrightText: 2021-2024 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

find_package(Grantlee5 REQUIRED)
message("-- Found Grantlee ${Grantlee5_VERSION}")

add_executable(
  doxlee
  doxml.h
  doxml.cpp
  main.cpp
  renderer.cpp
  renderer.h
  variant.cpp
  variant.h
)

target_link_libraries(
  doxlee
  PRIVATE Grantlee5::Templates
  PRIVATE Qt${QT_VERSION_MAJOR}::Core)
