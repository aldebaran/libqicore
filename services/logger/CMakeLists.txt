find_package(idl REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/../..)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

find_program(_doxygen doxygen)

if(_doxygen AND NOT QI_NO_DOXYGEN)

  qi_create_idl(idlfiles
    SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/loggermanager.hpp
    CLASSES LogListener LoggerManager
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/share/idl
    TYPE_MAP qi::LogLevel=i<qi::LogLevel>
  )

  qi_create_idl(idlfiles
    SRC ${CMAKE_CURRENT_SOURCE_DIR}/logger/logprovider.hpp
    CLASSES LogProvider
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/share/idl
    TYPE_MAP qi::LogLevel=i<qi::LogLevel>
  )
  set(IDL_DIR "${CMAKE_CURRENT_BINARY_DIR}")
else()
  set(IDL_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
endif()

# LoggerManager Proxy
qi_create_proxy(${IDL_DIR}/share/idl/LoggerManager.xml
  LoggerManager
  ${CMAKE_CURRENT_BINARY_DIR}
  _loggermanager_proxy
)

# LogProvider Proxy
qi_create_proxy(${IDL_DIR}/share/idl/LogProvider.xml
  LogProvider
  ${CMAKE_CURRENT_BINARY_DIR}
  _logprovider_proxy
)

# LogListener Proxy
qi_create_proxy(${IDL_DIR}/share/idl/LogListener.xml
  LogListener
  ${CMAKE_CURRENT_BINARY_DIR}
  _loglistener_proxy
)

# loggermanager_bind.hpp
qi_create_binder(_loggermanager_bind
  IDL ${IDL_DIR}/share/idl/LoggerManager.xml
  DIR ${CMAKE_CURRENT_BINARY_DIR}
  NAME LoggerManager
  SERVICE
  INCLUDE logger/logger.hpp src/loggermanager.hpp
)

# logprovider_bind.cpp
qi_create_binder(_logprovider_bind
  CPP
  IDL ${IDL_DIR}/share/idl/LogProvider.xml
  DIR ${CMAKE_CURRENT_BINARY_DIR}
  NAME LogProvider
  INCLUDE logger/logger.hpp logger/logprovider.hpp
)

# loglistener_bind.cpp
qi_create_binder(_loglistener_bind
  CPP
  IDL ${IDL_DIR}/share/idl/LogListener.xml
  DIR ${CMAKE_CURRENT_BINARY_DIR}
  NAME LogListener
  INCLUDE logger/logger.hpp src/loggermanager.hpp
)


qi_create_lib(loggermanager
  SRC src/loggermanager.cpp logger/logger.hpp ${_loggermanager_proxy} ${_logprovider_proxy} ${_loggermanager_bind} ${_loglistener_bind} ${_loglistener_proxy}
  MODULE
  DEPENDS QITYPE
)

qi_create_lib(logprovider
  STATIC
  SRC logger/logger.hpp src/logprovider.cpp ${_loglistener_proxy}  ${_logprovider_bind}
  DEPENDS QI QITYPE
 )
qi_stage_lib(logprovider)

qi_stage_header_only_lib(logservice INCLUDE_DIRS
  "."
  ${CMAKE_CURRENT_BINARY_DIR}/../..
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )

qi_install_data(
  ${idlfiles}
  SUBFOLDER idl
  )

add_subdirectory("tests")
