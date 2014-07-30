## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.

cmake_minimum_required(VERSION 2.8)
project(LibQiCore)
find_package(qibuild)
qi_sanitize_compile_flags(HIDDEN_SYMBOLS)

# Tests
enable_testing()

add_subdirectory("libqicore")
add_subdirectory("services")
add_subdirectory("bin")

