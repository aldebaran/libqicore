function(qi_create_session_test name)
  if (DEFINED BUILD_TESTS AND NOT BUILD_TESTS)
    return()
  endif()
  cmake_parse_arguments(ARG "" "TIMEOUT" "ARGUMENTS;SRC;DEPENDS" ${ARGN})

  MATH( EXPR ARG_TIMEOUT_SSL "${ARG_TIMEOUT} * 2" )

  qi_create_gtest(${name} SRC ${ARG_SRC} DEPENDS ${ARG_DEPENDS} NO_ADD_TEST)
  qi_add_test(${name}_direct ${name} ARGUMENTS --mode=direct ${ARG_ARGUMENTS} TIMEOUT ${ARG_TIMEOUT})
# qi_add_test(${name}_nightmare ${name} ARGUMENTS --mode=nightmare ${ARG_ARGUMENTS} TIMEOUT ${ARG_TIMEOUT})
  qi_add_test(${name}_sd ${name} ARGUMENTS --mode=sd ${ARG_ARGUMENTS} TIMEOUT ${ARG_TIMEOUT})
  if (WITH_SSL)
  qi_add_test(${name}_ssl ${name} ARGUMENTS --mode=ssl ${ARG_ARGUMENTS} TIMEOUT ${ARG_TIMEOUT_SSL})
  endif()
endfunction()


qi_create_bin(dumper dump.cpp DEPENDS QI QICORE)
qi_create_session_test(test_logger SRC test_logger.cpp DEPENDS QI QICORE GTEST TESTSESSION TIMEOUT 10)
qi_create_bin(manager manager.cpp   DEPENDS QI QICORE TESTSESSION)
qi_create_bin(provider provider.cpp DEPENDS QI QICORE TESTSESSION)
qi_create_bin(listener listener.cpp DEPENDS QI QICORE TESTSESSION)
