add_library(core SHARED)
add_library(qicore::core ALIAS core)

target_sources(
  core
  PUBLIC
    FILE_SET source_headers
      TYPE HEADERS
      FILES
        qicore/api.hpp
        qicore/loglistener.hpp
        qicore/logmanager.hpp
        qicore/logmessage.hpp
        qicore/logprovider.hpp
        qicore/file.hpp
        qicore/detail/fileoperation.hxx
  PRIVATE
    src/loglistener_proxy.cpp
    src/logmanager_proxy.cpp
    src/logprovider_proxy.cpp
    src/registration.cpp
    src/logproviderimpl.cpp
    src/logproviderimpl.hpp
    src/file_proxy.cpp
    src/fileimpl.cpp
    src/fileoperation.cpp
    src/progressnotifier.cpp
    src/progressnotifier_proxy.cpp
)

target_include_directories(
  core
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(
  core
  PUBLIC
    qi::qi
    Boost::filesystem
)

set_target_properties(
  core
    PROPERTIES
      OUTPUT_NAME qicore
      VERSION "${QI_CORE_VERSION}"
      SOVERSION "${QI_CORE_VERSION_MAJOR}"
      CXX_VISIBILITY_PRESET hidden
)

install(
  TARGETS core cxx_standard
  EXPORT core-targets
  RUNTIME COMPONENT runtime
  LIBRARY COMPONENT runtime
  FILE_SET source_headers COMPONENT devel
)

make_package_config_file(core-targets qicore COMPONENT devel)

if(BUILD_TESTING)
  add_subdirectory(tests)
  add_subdirectory(example)
endif()
