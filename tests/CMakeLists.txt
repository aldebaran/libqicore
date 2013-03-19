## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.

cmake_minimum_required(VERSION 2.8)

qi_create_gtest(test_timeline SRC test_timeline.cpp
                DEPENDS QICORE ALCOMMON ALLAUNCHER
                ARGUMENTS "${CMAKE_CURRENT_SOURCE_DIR}/behaviors/timeline/behavior.xar")

qi_create_gtest(test_statemachine test_statemachine.cpp DEPENDS qicore)
