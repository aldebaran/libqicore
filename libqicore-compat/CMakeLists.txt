## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.

cmake_minimum_required(VERSION 2.8)
project(LibQiCoreCompat)
find_package(qibuild)
qi_sanitize_compile_flags(HIDDEN_SYMBOLS)

include_directories(".")

set(PUBLIC_HEADERS
  qicore-compat/api.hpp
  qicore-compat/timeline.hpp
  qicore-compat/model/bitmapmodel.hpp
  qicore-compat/model/boxinterfacemodel.hpp
  qicore-compat/model/choicemodel.hpp
  qicore-compat/model/contentmodel.hpp
  qicore-compat/model/contentsmodel.hpp
  qicore-compat/model/inputmodel.hpp
  qicore-compat/model/outputmodel.hpp
  qicore-compat/model/parametermodel.hpp
  qicore-compat/model/resourcemodel.hpp
  qicore-compat/model/actuatorcurvemodel.hpp
  qicore-compat/model/actuatorlistmodel.hpp
  qicore-compat/model/animationmodel.hpp
  qicore-compat/model/keymodel.hpp
  qicore-compat/model/tangentmodel.hpp
  qicore-compat/model/behaviorkeyframemodel.hpp
  qicore-compat/model/behaviorlayermodel.hpp
  qicore-compat/model/behaviorsequencemodel.hpp
  qicore-compat/model/flowdiagrammodel.hpp
  qicore-compat/model/linkmodel.hpp
  qicore-compat/model/boxinstancemodel.hpp
  qicore-compat/model/parametervaluemodel.hpp
  qicore-compat/model/choregrapheprojectmodel.hpp
  qicore-compat/behaviorexecuter.hpp)

qi_install_header(${PUBLIC_HEADERS})

qi_create_lib(qicore_compat SHARED SRC
  ${PUBLIC_HEADERS}
  src/timeline_p.hpp
  src/timeline.cpp
  src/model/xmlutils.hpp
  src/asyncexecuter.hpp
  src/asyncexecuter.cpp
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
  src/model/actuatorcurvemodel_p.hpp
  src/model/actuatorcurvemodel.cpp
  src/model/actuatorlistmodel_p.hpp
  src/model/actuatorlistmodel.cpp
  src/model/animationmodel_p.hpp
  src/model/animationmodel.cpp
  src/model/keymodel_p.hpp
  src/model/keymodel.cpp
  src/model/tangentmodel_p.hpp
  src/model/tangentmodel.cpp
  src/model/behaviorkeyframemodel_p.hpp
  src/model/behaviorkeyframemodel.cpp
  src/model/behaviorlayermodel_p.hpp
  src/model/behaviorlayermodel.cpp
  src/model/behaviorsequencemodel_p.hpp
  src/model/behaviorsequencemodel.cpp
  src/model/flowdiagrammodel_p.hpp
  src/model/flowdiagrammodel.cpp
  src/model/linkmodel_p.hpp
  src/model/linkmodel.cpp
  src/model/boxinstancemodel_p.hpp
  src/model/boxinstancemodel.cpp
  src/model/parametervaluemodel_p.hpp
  src/model/parametervaluemodel.cpp
  src/model/choregrapheprojectmodel.cpp
  src/model/choregrapheprojectmodel_p.hpp
  src/python/pythonloader.hpp
  src/python/pythonloader.cpp
  src/python/pythonboxgenerator.hpp
  src/python/pythonboxgenerator.cpp
  src/behaviorexecuter.cpp
  src/behaviorexecuter_p.hpp)

qi_use_lib(qicore_compat QI QICORE QIPYTHON ALSERIAL ALMATHINTERNAL ALPROXIES BOOST_PYTHON)
qi_stage_lib(qicore_compat)

add_subdirectory("tests")
add_subdirectory("bin")
add_subdirectory("python")
