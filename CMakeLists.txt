cmake_minimum_required(VERSION 3.23)
project(qudev LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(QUDEV_BUILD_EXAMPLES "Build example applications" ON)
option(QUDEV_BUILD_TESTS    "Build tests" OFF)
option(QUDEV_BUILD_DOCS     "Enable Doxygen documentation target" ON)

find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBUDEV REQUIRED IMPORTED_TARGET libudev)

add_subdirectory(src)

if(QUDEV_BUILD_EXAMPLES)
  add_subdirectory(examples/udevviewer)
endif()

if(QUDEV_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

# ------------------------------------------------------------------------------
# Doxygen documentation target (visible in QtCreator as "doc")
# ------------------------------------------------------------------------------

if(QUDEV_BUILD_DOCS)
  find_package(Doxygen QUIET)

  if(DOXYGEN_FOUND)
    message(STATUS "Doxygen found: ${DOXYGEN_EXECUTABLE}")

    add_custom_target(docs
      # Ensure the output directory exists (Doxygen won't create the parent "docs")
      COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/docs/html
      COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      COMMENT "Generating API documentation with Doxygen"
      VERBATIM
    )
  else()
    message(STATUS "Doxygen not found, 'doc' target will not be available")
  endif()
endif()
