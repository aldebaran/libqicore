##
## Author(s):
##  - Cedric GESTES <gestes@aldebaran-robotics.com>
##
## Copyright (C) 2013 Aldebaran Robotics
##

#call qipkg package
#add the correct install rules
# This is very simple and experimental
function(qi_package NAME)
  find_program(QIPKG_BIN qipkg)
  message(MESSAGE "qipkg: ${QIPKG_BIN}")

  add_custom_command(
    OUTPUT  "${CMAKE_CURRENT_BINARY_DIR}/${NAME}.pkg"
    COMMAND "${QIPKG_BIN}" package "${CMAKE_CURRENT_BINARY_DIR}/"
    )
  add_custom_target(${NAME}.pkg ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${NAME}.pkg")
  qi_install_data("${CMAKE_CURRENT_BINARY_DIR}/${NAME}.pkg" SUBFOLDER "apps")
endfunction()
