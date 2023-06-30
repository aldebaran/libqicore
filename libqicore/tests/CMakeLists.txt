add_executable(
  test_file
  test_file.cpp
)

target_link_libraries(
  test_file
  PRIVATE
    qicore::core
    qi::qi
    qi::testsession
    GTest::gtest
)

add_test(NAME test_file COMMAND test_file)

add_executable(
  send_robot_icon
  send_robot_icon.cpp
)

target_link_libraries(
  send_robot_icon
  PRIVATE
    qicore::core
)
