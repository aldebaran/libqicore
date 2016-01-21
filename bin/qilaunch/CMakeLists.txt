# Copyright (C) 2015 Aldebaran Robotics

project(QiLaunch)

qi_create_bin(qilaunch
  qilaunch.cpp
  DEPENDS QI QICORE BOOST_PROGRAM_OPTIONS QT5_CORE
  )

qi_create_module(qilaunchtestmodule SRC qilaunchtestmodule.cpp DEPENDS QI NO_INSTALL)

qi_add_test(qilaunch_func qilaunch ARGUMENTS --qi-standalone -n testlauncher -f qilaunchtestmodule.func)
qi_add_test(qilaunch_func_session qilaunch ARGUMENTS --qi-standalone -n testlauncher -f qilaunchtestmodule.funcWithSession)

qi_add_test(
  qilaunch_func_args

  qilaunch
  ARGUMENTS
  --qi-log-level 6
  --qi-standalone
  -n testlauncher
  -f "qilaunchtestmodule.funcWithArgs(3.14159, \"un string\", {\"peripateticien\": \"aristote\", \"dodecahedre\": 42})"
)

qi_add_test(
  qilaunch_func_session_args

  qilaunch
  ARGUMENTS
  --qi-log-level 6
  --qi-standalone
  -n testlauncher
  -f "qilaunchtestmodule.funcWithSessionAndArgs(3.14159, \"un string\", {\"peripateticien\": \"aristote\", \"dodecahedre\": 42})"
)

qi_add_test(qilaunch_service qilaunch ARGUMENTS --qi-standalone -n testlauncher -o qilaunchtestmodule.MyService -f qilaunchtestmodule.testMyService)
