## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.

include_directories(".")

set(PUBLIC_HEADERS
    qicore/api.hpp
    qicore/behavior.hpp
    qicore/loglistener.hpp
    qicore/logmanager.hpp
    qicore/logmessage.hpp
    qicore/logprovider.hpp
    )
qi_install_header(${PUBLIC_HEADERS})

qi_create_lib(qicore SHARED SRC
  ${PUBLIC_HEADERS}
  src/behavior.cpp src/behaviormodel.cpp src/format_behavior_model.cpp
  src/loglistener_proxy.cpp src/logmanager_proxy.cpp src/logprovider_proxy.cpp
  DEPENDS BOOST QI QITYPE QIMESSAGING
)
qi_use_lib(qicore qitype qilang)

qi_stage_lib(qicore)

qi_create_lib(logprovider
  SHARED
  SRC
  src/logproviderimpl.cpp
  src/logproviderimpl.hpp
  DEPENDS BOOST QI QITYPE QIMESSAGING QICORE
  )
qi_stage_lib(logprovider)
