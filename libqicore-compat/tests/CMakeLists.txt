## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.

cmake_minimum_required(VERSION 2.8)

# needs a naoqi server to be run
qi_create_test_helper(test_timeline SRC test_timeline.cpp
                      DEPENDS GTEST QICORE_COMPAT ALCOMMON ALLAUNCHER)
                      #ARGUMENTS "${CMAKE_CURRENT_SOURCE_DIR}/behaviors/timeline/behavior.xar")

qi_create_gtest(test_xmlparser SRC test_xmlparser.cpp
                DEPENDS qicore_compat
                ARGUMENTS "${CMAKE_CURRENT_SOURCE_DIR}/behaviors/xmlparser")

# needs a naoqi server to be run
qi_create_test_helper(test_behaviorexecuter SRC test_behaviorexecuter.cpp
                      DEPENDS GTEST QICORE_COMPAT ALLAUNCHER)
                      #ARGUMENTS "${CMAKE_CURRENT_SOURCE_DIR}/behaviors/executesay" "${CMAKE_CURRENT_SOURCE_DIR}/behaviors/angry_1")
