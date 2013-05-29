# REMOVE ME
add_definitions( -DWITH_SOAPDEFS_H -DWITH_NOIDREF )
add_definitions( -DNETWORK_HEARTBEAT )
#

include(qibuild/swig/python)

set (_pyfiles
  "${CMAKE_CURRENT_SOURCE_DIR}/py/qicore/qicore_legacy.py"
  "${CMAKE_CURRENT_SOURCE_DIR}/py/qicore/object_factory.py"
)

qi_install_python(${_pyfiles})

# FIXME: maybe use a "configure" here?
add_custom_target(copy-python-naoqi ALL
  COMMAND ${CMAKE_COMMAND}
  -E copy_directory
  "${CMAKE_CURRENT_SOURCE_DIR}/py/"
  "${QI_SDK_DIR}/${QI_SDK_LIB}"
  )
