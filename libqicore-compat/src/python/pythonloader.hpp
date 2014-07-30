/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2013 All Rights Reserved
*/

#ifndef PYTHONBOXCREATOR_HPP_
#define PYTHONBOXCREATOR_HPP_

#include <string>
#include <boost/python.hpp>
#include <qi/session.hpp>
#include "pythonboxgenerator.hpp"

namespace qi
{
  namespace py
  {
    using namespace boost::python;
  }

  class PythonBoxLoader
  {
  public:
    PythonBoxLoader();

    bool registerPythonClass(BoxInstanceModelPtr instance);
    void initPython(const std::string &ip, const std::string &port, const std::string &dir, boost::shared_ptr<qi::Session> session);
    void terminate();

  private:
    py::object _main;
    py::object _mainNamespace;
  };
}

#endif /* !PYTHONBOXCREATOR_HPP_ */
