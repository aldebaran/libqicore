add_library(alice_service STATIC)

target_sources(
  alice_service
  PUBLIC
    api.hpp
    imagestore.hpp
  PRIVATE
    imagestore.cpp
    imagestore_proxy.cpp
)

target_link_libraries(
  alice_service
  PUBLIC
    qicore::internal::cxx_standard
    qicore::core
    qi::qi
    Boost::filesystem
)

add_executable(file_example)

target_sources(
  file_example
  PRIVATE
    file_example.cpp
)

target_link_libraries(
  file_example
  PRIVATE
    alice_service
    qicore::core
    qi::qi
    qi::testsession
)
