find_package(Grantlee5 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)

add_executable(
  testRenderer
  testrenderer.h
  testrenderer.cpp
  ${CMAKE_SOURCE_DIR}/src/renderer.h
  ${CMAKE_SOURCE_DIR}/src/renderer.cpp)

add_test(NAME testRenderer COMMAND testRenderer)

target_include_directories(
  testRenderer
  PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(
  testRenderer
  PRIVATE Grantlee5::Templates
  PRIVATE Qt${QT_VERSION_MAJOR}::Test
)