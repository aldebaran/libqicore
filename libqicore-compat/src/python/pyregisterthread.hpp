/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2013 All Rights Reserved
*/

#ifndef PYREGISTERTHREAD_HPP_
#define PYREGISTERTHREAD_HPP_

#include <boost/python.hpp>

namespace qi
{
  class PyRegisterThread {
  public:
    /* Register thread in python world                  *
     * If a thread try to execute a python without this *
     * the thread block on the GIL_Ensure method        */
    PyRegisterThread(PyInterpreterState *interpreter);
    ~PyRegisterThread();

  private:
    PyThreadState* _state;
  };
}

#endif /* !PYREGISTERTHREAD_HPP_ */
