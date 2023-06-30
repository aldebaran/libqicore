## qicore CMake project
## ================
##
## Parameters:
## -----------
## QI_CORE_VERSION
## ~~~~~~~~~~
## A version number that will be used for this project. If this variable is
## undefined, CMake will read the project version number from the
## `project.json` file in the source directory.
##
## BUILD_TESTING
## ~~~~~~~~~~~~~
## If set to true, enables building the tests. See the documentation of the
## `CTest` module for more details on this variable.
##
## Targets
## -------
## The targets defined by the project are:
## - `qicore::core`: shared library.

cmake_minimum_required(VERSION 3.23)

# - Parse project description file
file(READ "${CMAKE_CURRENT_LIST_DIR}/project.json" project_json)

# - Define the project version number.
if(NOT QI_CORE_VERSION)
  string(JSON QI_CORE_VERSION GET "${project_json}" version)
endif()

include(cmake/ParseVersion.cmake)
parse_version(QI_CORE "${QI_CORE_VERSION}")
message(STATUS "Project version is ${QI_CORE_VERSION_FULL} (interpreted as ${QI_CORE_VERSION} for CMake)")

project(qicore VERSION "${QI_CORE_VERSION}")

include(CTest)
include(cmake/BuildType.cmake)
include(cmake/MakePackageConfigFile.cmake)

set(Boost_COMPONENTS filesystem thread)
find_package(
  Boost REQUIRED
  COMPONENTS
    ${Boost_COMPONENTS}
)

find_package(qi REQUIRED)

if(BUILD_TESTING)
    find_package(GTest REQUIRED)
    include(GoogleTest)
endif()

##############################################################################
# Convenience library: cxx_standard
##############################################################################
add_library(cxx_standard INTERFACE)
add_library(qicore::internal::cxx_standard ALIAS cxx_standard)

# The project requires at least C++17.
target_compile_features(
  cxx_standard
  INTERFACE
    cxx_std_17
)

set_target_properties(
  cxx_standard
  PROPERTIES
    EXPORT_NAME internal::cxx_standard
)

add_subdirectory(libqicore)
