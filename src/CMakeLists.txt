# --- Library target ---
add_library(qudev
  qudev.cpp
  qudev_context.cpp
  qudev_enumerator.cpp
  qudev_monitor.cpp
  qudev_device.cpp
)

add_library(qudev::qudev ALIAS qudev)

set_target_properties(qudev PROPERTIES
  AUTOMOC ON
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
)

# --- Public header file set: ONLY the umbrella header ---
target_sources(qudev
  PUBLIC
    FILE_SET public_headers TYPE HEADERS
      BASE_DIRS ${PROJECT_SOURCE_DIR}/include
      FILES
        ${PROJECT_SOURCE_DIR}/include/qudev.h
        ${PROJECT_SOURCE_DIR}/include/qudev_filters.h
        ${PROJECT_SOURCE_DIR}/include/qudev_device.h
  PRIVATE
    # Private headers (shown in IDE, not installed as public)
    qudev_context.h
    qudev_enumerator.h
    qudev_monitor.h
)

# Public include dir (for consumers): only 'include'
# Private include dir: this src folder (internals)
target_include_directories(qudev
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Deps
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBUDEV REQUIRED IMPORTED_TARGET libudev)

target_link_libraries(qudev
  PUBLIC  Qt6::Core
  PRIVATE PkgConfig::LIBUDEV
)
