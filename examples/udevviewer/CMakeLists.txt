# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2025 Georgi Georgiev, Samsa Ltd. <georgi@samsa.io>
#
# qudev - Qt wrapper around libudev
#
# This file is part of the qudev project.
# See the LICENSE file in the project root for full license text.

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)

add_executable(udevviewer
  main.cpp
  qudev_service.cpp
  qudev_service.h
  qudev_device_model.cpp
  qudev_device_model.h
  qudev_filters_model.cpp
  qudev_filters_model.h
  qudev_device_search_model.cpp
  qudev_device_search_model.h
  resources.qrc
)

set_target_properties(udevviewer PROPERTIES
  AUTOMOC ON
  AUTORCC ON
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
)

target_link_libraries(udevviewer PRIVATE
  Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick
  qudev::qudev
)
