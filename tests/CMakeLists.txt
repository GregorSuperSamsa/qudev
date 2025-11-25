# Minimal Qt Test setup

if(QT_VERSION_MAJOR EQUAL 6)
  find_package(Qt6 REQUIRED COMPONENTS Core Test)
  set(QUDEV_TEST_QT_CORE Qt6::Core)
  set(QUDEV_TEST_QT_TEST Qt6::Test)
else()
  find_package(Qt5 REQUIRED COMPONENTS Core Test)
  set(QUDEV_TEST_QT_CORE Qt5::Core)
  set(QUDEV_TEST_QT_TEST Qt5::Test)
endif()

function(qudev_add_test name)
  add_executable(${name} ${ARGN})
  set_target_properties(${name} PROPERTIES AUTOMOC ON CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
  target_link_libraries(${name} PRIVATE qudev::qudev ${QUDEV_TEST_QT_CORE} ${QUDEV_TEST_QT_TEST})
  add_test(NAME ${name} COMMAND ${name})
endfunction()

qudev_add_test(test_enumerator test_enumerator.cpp)
qudev_add_test(test_monitor    test_monitor.cpp)
qudev_add_test(test_device_model test_device_model.cpp)

