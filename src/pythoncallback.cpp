/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <qi/log.hpp>

#include "pythoncallback.hpp"

PythonCallback::PythonCallback(PyObject* p)
  : _callback(p)
{
  Py_XINCREF(_callback);
}

PythonCallback::~PythonCallback()
{
  Py_XDECREF(_callback);
}

void PythonCallback::operator ()()
{
  if (!_callback)
    return;

  PyObject* ret;
  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();

  ret = PyObject_CallFunctionObjArgs(_callback, NULL);
  if (!ret)
  {
    qiLogError("qiCore.PythonCallback") << "PyhtonCallback is unable to call python callback";
    PyErr_Print();
    PyErr_Clear();
  }

  Py_XDECREF(ret);

  PyGILState_Release(gstate);
}

void PythonCallback::assignCallback(PyObject *p)
{
  Py_XDECREF(_callback);
  _callback = p;
  Py_XINCREF(_callback);
}
