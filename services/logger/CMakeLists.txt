include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

qi_create_module(logmanager
  MODULE
  SRC
  src/loglistenerimpl.cpp
  src/loglistenerimpl.hpp
  src/logmanagerimpl.cpp
  src/logmanagerimpl.hpp
  DEPENDS BOOST QI QICORE
  )

add_subdirectory("tests")
