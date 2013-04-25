## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.

include_directories(".")

set(PUBLIC_HEADERS
  qicore-compat/api.hpp
  qicore-compat/timeline.hpp
  qicore-compat/statemachine.hpp
  qicore-compat/box.hpp
  qicore-compat/transition.hpp
  qicore-compat/model/bitmapmodel.hpp
  qicore-compat/model/boxinterfacemodel.hpp
  qicore-compat/model/choicemodel.hpp
  qicore-compat/model/contentmodel.hpp
  qicore-compat/model/contentsmodel.hpp
  qicore-compat/model/inputmodel.hpp
  qicore-compat/model/outputmodel.hpp
  qicore-compat/model/parametermodel.hpp
  qicore-compat/model/resourcemodel.hpp)

qi_install_header(${PUBLIC_HEADERS})

qi_create_lib(qicore-compat SHARED SRC
  ${PUBLIC_HEADERS}
  src/timeline_p.hpp
  src/timeline.cpp
  src/model/xmlutils.hpp
  src/model/xmlutils.cpp
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
  src/pythoncallback.cpp
  src/model/bitmapmodel_p.hpp
  src/model/bitmapmodel.cpp
  src/model/boxinterfacemodel_p.hpp
  src/model/boxinterfacemodel.cpp
  src/model/choicemodel_p.hpp
  src/model/choicemodel.cpp
  src/model/contentmodel_p.hpp
  src/model/contentmodel.cpp
  src/model/contentsmodel_p.hpp
  src/model/contentsmodel.cpp
  src/model/inputmodel_p.hpp
  src/model/inputmodel.cpp
  src/model/outputmodel_p.hpp
  src/model/outputmodel.cpp
  src/model/parametermodel_p.hpp
  src/model/parametermodel.cpp
  src/model/resourcemodel_p.hpp
  src/model/resourcemodel.cpp
  src/serial.hpp
  src/serial.cpp)

qi_use_lib(qicore-compat QI ALSERIAL ALMATHINTERNAL ALPROXIES PYTHON)
qi_stage_lib(qicore-compat)

add_subdirectory("python")
add_subdirectory("tests")
