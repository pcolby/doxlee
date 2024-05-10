# SPDX-FileCopyrightText: 2021-2024 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

set(DoxleeSources
  doxml.h
  doxml.cpp
  renderer.cpp
  renderer.h
  textlee.h
  variant.cpp
  variant.h
)

# All but main.cpp in a shared 'object' library so unit tests can link against the same binary objects.
add_library(libDoxlee OBJECT ${DoxleeSources})

target_link_libraries(
  libDoxlee
  PRIVATE ${TEXT_TEMPLATE_TARGET}
  PRIVATE Qt${QT_VERSION_MAJOR}::Core)

add_executable(doxlee main.cpp)

# \todo Add a custom Info.plist file to the app bundle.

# \todo qt_add_translations

target_link_libraries(
  doxlee
  PRIVATE libDoxlee
  PRIVATE ${TEXT_TEMPLATE_TARGET}
  PRIVATE Qt${QT_VERSION_MAJOR}::Core)

# \todo linuxdeploy/appImage, macdeployqt, etc.
