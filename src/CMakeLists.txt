find_package(Grantlee5 REQUIRED)
message("-- Found Grantlee ${Grantlee5_VERSION}")

# \todo Use add_compile_definitions if we ever have a compelling reason to require CMake 3.12+
add_definitions(
  -DCMAKE_PROJECT_NAME="${CMAKE_PROJECT_NAME}"
  -DCMAKE_PROJECT_VERSION="${CMAKE_PROJECT_VERSION}")

add_executable(
  doxlee
  main.cpp
  renderer.cpp
  renderer.h
)

target_link_libraries(
  doxlee
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Grantlee5::Templates)
