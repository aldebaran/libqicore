/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef PYTHONCALLBACK_H_
# define PYTHONCALLBACK_H_

# include <Python.h>

# include <qi/macro.hpp>

class PythonCallback
{
  public:
    PythonCallback(PyObject*);
    ~PythonCallback();

    void operator()();
    void assignCallback(PyObject*);

  private:
    PyObject* _callback;

/* Protect the ref counting of the callback */
QI_DISALLOW_COPY_AND_ASSIGN(PythonCallback);
};

#endif /* !PYTHONCALLBACK_H_ */
