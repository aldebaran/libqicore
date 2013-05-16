## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.

include_directories(".")

set(PUBLIC_HEADERS
    qicore/api.hpp
    qicore/behavior.hpp)

qi_install_header(${PUBLIC_HEADERS})

qi_create_lib(qicore SHARED SRC
  ${PUBLIC_HEADERS}
  src/behavior.cpp)

qi_use_lib(qicore qitype)

qi_stage_lib(qicore)
