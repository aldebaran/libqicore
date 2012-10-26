# REMOVE ME
add_definitions( -DWITH_SOAPDEFS_H -DWITH_NOIDREF )
add_definitions( -DNETWORK_HEARTBEAT )

use(PYTHON-TOOLS)
#

include("qibuild/swig/python")

set (_pyfiles
  "${CMAKE_CURRENT_SOURCE_DIR}/py/newnaoqi.py"
  "${CMAKE_CURRENT_SOURCE_DIR}/py/motion.py"
  "${CMAKE_CURRENT_SOURCE_DIR}/py/vision_definitions.py"
)

install(FILES
        ${_pyfiles}
        COMPONENT python
        DESTINATION ${_SDK_LIB}
        )

# FIXME: maybe use a "configure" here?
add_custom_target(copy-python-naoqi ALL
  COMMAND ${CMAKE_COMMAND}
  -E copy_directory
  "${CMAKE_CURRENT_SOURCE_DIR}/py/"
  "${SDK_DIR}/${_SDK_LIB}"
  )

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/src")
qi_swig_wrap_python(newframemanager src/newframemanager.i DEPENDS NEWFRAMEMANAGER)

#REMOVE ME
qi_swig_wrap_python(newinaoqi src/newinaoqi.i
                              ../newframemanager/newinaoqi.h
                              ../src/newinaoqi.cpp
                              ../src/newinaoqi_private.h
                              DEPENDS       ALCOMMON
                                            ALPYTHONTOOLS
                                            ALBEHAVIOR
                                            ALRESOURCE
                                            ALPROXIES
)
#
