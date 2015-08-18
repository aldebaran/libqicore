# Copyright (C) 2015 Aldebaran Robotics

project(QiLaunch)

qi_create_bin(qilaunch
  qilaunch.cpp
  DEPENDS QI QICORE BOOST_PROGRAM_OPTIONS
  )

qi_create_module(qilaunchtestmodule SRC qilaunchtestmodule.cpp DEPENDS QI NO_INSTALL)

qi_add_test(qilaunch_func qilaunch ARGUMENTS --qi-standalone -n testlauncher -f qilaunchtestmodule.func)
qi_add_test(qilaunch_service qilaunch ARGUMENTS --qi-standalone -n testlauncher -o qilaunchtestmodule.MyService -f qilaunchtestmodule.testMyService)
