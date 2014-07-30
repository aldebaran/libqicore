## Copyright (c) 2013 Aldebaran Robotics. All rights reserved.

add_subdirectory("logger")

qi_add_optional_package(QT_QTCORE "Enable QT")
qi_add_optional_package(QT_QTGUI  "Enable QTGUI")
if (WITH_QT_QTCORE AND WITH_QT_QTGUI)
#  add_subdirectory("tray")
endif()
