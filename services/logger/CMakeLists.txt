find_package(idl REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/../..)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

find_program(_doxygen doxygen)

if(_doxygen AND NOT QI_NO_DOXYGEN)

  qi_create_idl(idlfiles
    SRC ${CMAKE_CURRENT_SOURCE_DIR}/logger/logger_service.hpp
    CLASSES LogListener Logger
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

# Logger Proxy
qi_create_proxy(${IDL_DIR}/share/idl/Logger.xml
  Logger
  ${CMAKE_CURRENT_BINARY_DIR}
  _logger_proxy
)

# Logger Proxy
qi_create_proxy(${IDL_DIR}/share/idl/LogProvider.xml
  LogProvider
  ${CMAKE_CURRENT_BINARY_DIR}
  _logprovider_proxy
)

# Logger Proxy
qi_create_proxy(${IDL_DIR}/share/idl/LogListener.xml
  LogListener
  ${CMAKE_CURRENT_BINARY_DIR}
  _loglistener_proxy
)

# logger_bind.hpp
qi_create_binder(_logger_bind
  IDL ${IDL_DIR}/share/idl/Logger.xml
  DIR ${CMAKE_CURRENT_BINARY_DIR}
  NAME Logger
  SERVICE
  INCLUDE logger/logger.hpp logger/logger_service.hpp
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
  INCLUDE logger/logger.hpp logger/logger_service.hpp
)


qi_create_lib(logger
  SRC src/logger.cpp logger/logger.hpp ${_logger_proxy} ${_logprovider_proxy} ${_logger_bind} ${_loglistener_bind} ${_loglistener_proxy}
  MODULE
  DEPENDS QITYPE
)

qi_create_lib(logprovider
  STATIC
  SRC src/logprovider.cpp ${_loglistener_proxy}  ${_logprovider_bind}
  DEPENDS QI QITYPE
 )
 qi_stage_lib(logprovider)

 qi_install_data(
   ${idlfiles}
   SUBFOLDER idl
   )

add_subdirectory("tests")
