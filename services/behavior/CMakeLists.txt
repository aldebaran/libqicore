qi_create_lib(behavior
  MODULE
  SRC behavior.cpp
  DEPENDS QIMESSAGING
  )
qi_use_lib(behavior qicore)

add_subdirectory(tests)
