## Copyright (c) 2012, 2014 Aldebaran Robotics. All rights reserved.

find_package(qimodule)
include_directories(".")

set(PUBLIC_HEADERS
    qicore/api.hpp
    #qicore/behavior.hpp
    qicore/loglistener.hpp
    qicore/logmanager.hpp
    qicore/logmessage.hpp
    qicore/logprovider.hpp
    )
qi_install_header(${PUBLIC_HEADERS})

qi_create_module(qicore SRC
  ${PUBLIC_HEADERS}
  #src/behavior.cpp src/behaviormodel.cpp src/format_behavior_model.cpp
  src/loglistener_proxy.cpp
  src/logmanager_proxy.cpp
  src/logprovider_proxy.cpp
  src/registration.cpp
  src/logproviderimpl.cpp
  src/logproviderimpl.hpp
  DEPENDS BOOST
)
qi_use_lib(qicore QI)
qi_stage_lib(qicore)

add_subdirectory("tests")
