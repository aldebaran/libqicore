## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.

cmake_minimum_required(VERSION 2.8)
project(LibQiCore)
find_package(qibuild)
qi_sanitize_compile_flags(HIDDEN_SYMBOLS)

include_directories(".")

set(PUBLIC_HEADERS
    qicore/api.hpp
    qicore/timeline.hpp
    qicore/state.hpp
    qicore/state_machine.hpp
    qicore/box.hpp
    qicore/diagram.hpp
    qicore/transition.hpp)

qi_install_header(${PUBLIC_HEADERS})

qi_create_lib(qicore SRC
  ${PUBLIC_HEADERS}
  src/timeline_private.hpp src/timeline.cpp
  src/xml_utils.hpp src/xml_utils.cpp
  src/actuator_curve.hpp src/actuator_curve.cpp
  src/state_machine_private.hpp src/state_machine.cpp
  src/state_private.hpp src/state.cpp
  src/transition_private.hpp src/transition.cpp
  src/async_executer.hpp src/async_executer.cpp
  src/box_private.hpp src/box.cpp
  src/diagram_private.hpp src/diagram.cpp)

qi_use_lib(qicore ALCOMMON ALSERIAL ALMATHINTERNAL ALBEHAVIOR ALTOOLS
#REMOVE ME OR NOT?
ALPYTHONTOOLS
#
)

qi_install(tests/test_state_machine.cpp DESTINATION ".")

qi_stage_lib(qicore)

qi_create_gtest(test_timeline SRC tests/test_timeline.cpp
                DEPENDS QICORE ALCOMMON ALLAUNCHER
                ARGUMENTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/projects/timeline/behavior.xar")

#qi_create_gtest(test_legacy_state_machine test/test_legacy_state_machine.cpp DEPENDS qicore)
#qi_create_gtest(test_state_machine test/test_state_machine.cpp DEPENDS qicore)

add_subdirectory("python")
