## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.

cmake_minimum_required(VERSION 2.8)
project(LibQiCore)
find_package(qibuild)
qi_sanitize_compile_flags(HIDDEN_SYMBOLS)

include_directories(".")

set(PUBLIC_HEADERS
    qicore/api.hpp
    qicore/timeline.hpp
    qicore/statemachine.hpp
    qicore/box.hpp
    qicore/transition.hpp)

qi_install_header(${PUBLIC_HEADERS})

qi_create_lib(qicore SRC
  ${PUBLIC_HEADERS}
  src/timeline_private.hpp src/timeline.cpp
  src/xmlutils.hpp src/xmlutils.cpp
  src/actuatorcurve.hpp src/actuatorcurve.cpp
  src/statemachine_private.hpp src/statemachine.cpp
  src/transition_private.hpp src/transition.cpp
  src/asyncexecuter.hpp src/asyncexecuter.cpp
  src/box_private.hpp src/box.cpp)

qi_use_lib(qicore QI ALSERIAL ALMATHINTERNAL ALPROXIES PYTHON)

qi_install(tests/test_statemachine.cpp DESTINATION ".")

qi_stage_lib(qicore)

add_subdirectory("python")
add_subdirectory("tests")
