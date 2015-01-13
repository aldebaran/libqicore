# Copyright (C) 2015 Aldebaran Robotics

project(QiLaunch)

qi_create_bin(qilaunch
  qilaunch.cpp
  DEPENDS QI QICORE BOOST_PROGRAM_OPTIONS
  )
