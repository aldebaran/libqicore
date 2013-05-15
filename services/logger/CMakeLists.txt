find_package(idl REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/../..)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
qi_create_idl(idlfiles
  SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/logger_service.hpp
  CLASSES LogListener Logger LogProvider
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/share/idl
)
message("IDL files: ${idlfiles}")

qi_create_proxy(${CMAKE_CURRENT_BINARY_DIR}/share/idl/Logger.xml
  Logger
  ${CMAKE_CURRENT_BINARY_DIR}
  _logger_proxy
)

qi_create_proxy(${CMAKE_CURRENT_BINARY_DIR}/share/idl/LogProvider.xml
  LogProvider
  ${CMAKE_CURRENT_BINARY_DIR}
  _logprovider_proxy
)

qi_create_proxy(${CMAKE_CURRENT_BINARY_DIR}/share/idl/LogListener.xml
  LogListener
  ${CMAKE_CURRENT_BINARY_DIR}
  _loglistener_proxy
)

# logger_bind.hpp
qi_create_binder(_logger_bind
  IDL ${CMAKE_CURRENT_BINARY_DIR}/share/idl/Logger.xml
  DIR ${CMAKE_CURRENT_BINARY_DIR}
  NAME Logger
  SERVICE
)

#logprovider_bind.cpp
qi_create_binder(_logprovider_bind
  CPP
  IDL ${CMAKE_CURRENT_BINARY_DIR}/share/idl/LogProvider.xml
  DIR ${CMAKE_CURRENT_BINARY_DIR}
  NAME LogProvider
  INCLUDE src/logger_service.hpp
)

qi_create_binder(_loglistener_bind
  CPP
  IDL ${CMAKE_CURRENT_BINARY_DIR}/share/idl/LogListener.xml
  DIR ${CMAKE_CURRENT_BINARY_DIR}
  NAME LogListener
  INCLUDE src/logger_service.hpp
)


qi_create_lib(logger
  SRC src/logger.cpp src/logger.hpp ${_logger_proxy} ${_logprovider_proxy} ${_logger_bind} ${_loglistener_bind}
  MODULE
  DEPENDS QITYPE
)

qi_create_lib(loggerclient
  STATIC
  SRC src/logprovider.cpp ${_loglistener_proxy}  ${_logprovider_bind}
  DEPENDS QI QITYPE
 )

 qi_stage_lib(loggerclient)

 qi_install_data(
   ${idlfiles}
   SUBFOLDER idl
   )
 add_subdirectory("tests")

