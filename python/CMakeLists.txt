# REMOVE ME
add_definitions( -DWITH_SOAPDEFS_H -DWITH_NOIDREF )
add_definitions( -DNETWORK_HEARTBEAT )
#

include(qibuild/swig/python)

set (_pyfiles
  "${CMAKE_CURRENT_SOURCE_DIR}/py/qicoreLegacy.py"
  "${CMAKE_CURRENT_SOURCE_DIR}/py/objectFactory.py"
  "${CMAKE_CURRENT_SOURCE_DIR}/py/converter/xar_converter.py"
)

install( FILES
  ${_pyfiles}
  COMPONENT python
  DESTINATION ${QI_SDK_LIB}
)

# FIXME: maybe use a "configure" here?
add_custom_target(copy-python-naoqi ALL
  COMMAND ${CMAKE_COMMAND}
  -E copy_directory
  "${CMAKE_CURRENT_SOURCE_DIR}/py/"
  "${QI_SDK_DIR}/${QI_SDK_LIB}"
  )

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/src")
qi_swig_wrap_python(qicore src/qicore.i
                           DEPENDS QICORE)
