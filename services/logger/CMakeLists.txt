find_package(idl REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/../..)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

find_program(_doxygen doxygen)

if(_doxygen AND NOT QI_NO_DOXYGEN)

  qi_create_idl(idlfiles
    SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/logmanager.hpp
    CLASSES LogListener LogManager
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/share/idl
    TYPE_MAP qi::LogLevel=i<qi::LogLevel>
  )

  qi_create_idl(idlfiles
    SRC ${CMAKE_CURRENT_SOURCE_DIR}/qicore/logprovider.hpp
    CLASSES LogProvider
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/share/idl
    TYPE_MAP qi::LogLevel=i<qi::LogLevel>
  )
  set(IDL_DIR "${CMAKE_CURRENT_BINARY_DIR}")
else()
  set(IDL_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
endif()

# LogManager Proxy
qi_create_proxy(${IDL_DIR}/share/idl/LogManager.xml
  LogManager
  ${CMAKE_CURRENT_BINARY_DIR}
  _logmanager_proxy
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

# logmanager_bind.hpp
qi_create_binder(_logmanager_bind
  IDL ${IDL_DIR}/share/idl/LogManager.xml
  DIR ${CMAKE_CURRENT_BINARY_DIR}
  NAME LogManager
  SERVICE
  INCLUDE qicore/logmessage.hpp src/logmanager.hpp
)

# logprovider_bind.cpp
qi_create_binder(_logprovider_bind
  CPP
  IDL ${IDL_DIR}/share/idl/LogProvider.xml
  DIR ${CMAKE_CURRENT_BINARY_DIR}
  NAME LogProvider
  INCLUDE qicore/logmessage.hpp qicore/logprovider.hpp
)

# loglistener_bind.cpp
qi_create_binder(_loglistener_bind
  CPP
  IDL ${IDL_DIR}/share/idl/LogListener.xml
  DIR ${CMAKE_CURRENT_BINARY_DIR}
  NAME LogListener
  INCLUDE qicore/logmessage.hpp src/logmanager.hpp
)


qi_create_lib(logmanager
  SRC src/logmanager.cpp qicore/logmessage.hpp qicore/logmanager_proxy.hpp qicore/logprovider_proxy.hpp qicore/logmanager_bind.hpp qicore/loglistener_bind.cpp qicore/loglistener_proxy.hpp
  MODULE
  DEPENDS QITYPE
)

qi_create_lib(logprovider
  STATIC
  SRC qicore/logmessage.hpp src/logprovider.cpp qicore/loglistener_proxy.hpp qicore/logprovider_bind.cpp
  DEPENDS QI QITYPE QICORE
 )
qi_stage_lib(logprovider)

qi_stage_header_only_lib(logservice INCLUDE_DIRS
  "."
  )

qi_install_data(
  ${idlfiles}
  SUBFOLDER idl
  )

add_subdirectory("tests")
