if (NOT I_AM_A_ROBOT)
  qi_create_bin(tray
    SRC tray.cpp
    DEPENDS QI QT_QTCORE QT_QTGUI
    )
endif()
