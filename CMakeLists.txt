## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.

cmake_minimum_required(VERSION 2.8)
project(LibQiCore)
find_package(qibuild)
qi_sanitize_compile_flags(HIDDEN_SYMBOLS)

include_directories(".")

set(PUBLIC_HEADERS
    qicore/timeline.h
    qicore/state.h
    qicore/state_machine.h
    qicore/box.h
    qicore/diagram.h
    qicore/transition.h
    qicore/newinaoqi.h)

qi_create_lib(qicore
  ${PUBLIC_HEADERS}
  src/timeline_private.h src/timeline.cpp
  src/xml_utils.h src/xml_utils.cpp
  src/actuator_curve.h src/actuator_curve.cpp
  src/state_machine_private.h src/state_machine.cpp
  src/state_private.h src/state.cpp
  src/transition_private.h src/transition.cpp
  src/async_executer.h src/async_executer.cpp
  src/box_private.h src/box.cpp
  src/newinaoqi_private.h src/newinaoqi.cpp
  src/diagram_private.h src/diagram.cpp)

qi_install_header(${PUBLIC_HEADERS})

qi_use_lib(qicore ALCOMMON ALSERIAL ALMATHINTERNAL ALBEHAVIOR ALTOOLS
#REMOVE ME OR NOT?
ALPYTHONTOOLS
#
)

qi_stage_lib(qicore qicore "${CMAKE_CURRENT_SOURCE_DIR}")

qi_create_bin(test_timeline NO_INSTALL tests/test_timeline.cpp)
qi_use_lib(test_timeline ALCOMMON ALLAUNCHER GTEST qicore)
qi_add_test(test_timeline test_timeline ARGUMENTS "${CMAKE_CURRENT_SOURCE_DIR}/test/projects/timeline/behavior.xar")

#qi_create_gtest(test_legacy_state_machine test/test_legacy_state_machine.cpp DEPENDS qicore)
#qi_create_gtest(test_state_machine test/test_state_machine.cpp DEPENDS qicore)

add_subdirectory("python")
