/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2013 All Rights Reserved
*/

#ifndef PYTHONBOXCREATOR_HPP_
#define PYTHONBOXCREATOR_HPP_

#include <string>
#include <boost/python.hpp>
#include "pythonboxgenerator.hpp"

namespace py = boost::python;

namespace qi
{
  class PythonBoxLoader
  {
  public:
    PythonBoxLoader();

    bool registerPythonClass(BoxInstanceModelPtr instance);
    void initPython(const std::string &ip, const std::string &port);

  private:
    py::object _main;
    py::object _mainNamespace;
  };
}

#endif /* !PYTHONBOXCREATOR_HPP_ */
