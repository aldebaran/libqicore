## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.

include_directories(".")

set(PUBLIC_HEADERS
    qicore-compat/api.hpp
    qicore-compat/timeline.hpp
    qicore-compat/statemachine.hpp
    qicore-compat/box.hpp
    qicore-compat/transition.hpp)

qi_install_header(${PUBLIC_HEADERS})

qi_create_lib(qicore-compat SHARED SRC
  ${PUBLIC_HEADERS}
  src/timeline_p.hpp
  src/timeline.cpp
  src/xmlutils.hpp
  src/xmlutils.cpp
  src/actuatorcurve.hpp
  src/actuatorcurve.cpp
  src/statemachine_p.hpp
  src/statemachine.cpp
  src/transition_p.hpp
  src/transition.cpp
  src/asyncexecuter.hpp
  src/asyncexecuter.cpp
  src/box_p.hpp
  src/box.cpp
  src/pythoncallback.hpp
  src/pythoncallback.cpp)

qi_use_lib(qicore-compat QI ALSERIAL ALMATHINTERNAL ALPROXIES PYTHON)
qi_stage_lib(qicore-compat)

add_subdirectory("python")
add_subdirectory("tests")
