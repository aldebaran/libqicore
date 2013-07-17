#include "pyregisterthread.hpp"

namespace qi {
  PyRegisterThread::PyRegisterThread(PyInterpreterState *interpreter)
  {
    _state = PyThreadState_New(interpreter);
    PyThreadState_Swap(_state);
  }

  PyRegisterThread::~PyRegisterThread()
  {
    PyThreadState_Swap(NULL);
    PyThreadState_Clear(_state);
    PyThreadState_Delete(_state);
  }
}
