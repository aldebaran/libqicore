/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2013 All Rights Reserved
*/

#include <fstream>

#include <boost/python.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <qi/log.hpp>
#include <qi/path.hpp>
#include <qitype/objectfactory.hpp>
#include <qipython/gil.hpp>

#include <qicore-compat/model/boxinterfacemodel.hpp>
#include <qicore-compat/model/boxinstancemodel.hpp>
#include <qicore-compat/model/contentmodel.hpp>
#include <qicore-compat/model/contentsmodel.hpp>
#include <qicore-compat/model/parametermodel.hpp>
#include <qicore-compat/model/behaviorsequencemodel.hpp>
#include <qicore-compat/model/behaviorlayermodel.hpp>
#include <qicore-compat/model/behaviorkeyframemodel.hpp>
#include <qicore-compat/model/resourcemodel.hpp>

#include "pythonloader.hpp"

#define foreach BOOST_FOREACH

qiLogCategory("QiCore-Compat.PythonBoxCreator");

namespace qi
{
  PythonBoxLoader::PythonBoxLoader()
  {
  }

  void PythonBoxLoader::initPython(const std::string &ip, const std::string &port, const std::string &dir)
  {
    PyEval_InitThreads();
    Py_Initialize();
    _mainThread = PyThreadState_Swap(NULL);
    // Py_Initialize takes the GIL, so we release it
    PyEval_ReleaseLock();

    // We will now execute python code, so we take the lock with a nice scoped
    // lock. This lock must be released at the end of the function because we
    // will call python function through qimessaging after that and qimessaging
    // will take the lock in other threads.
    py::GILScopedLock lock;
    //create main namespace
    _main = py::import("__main__");
    _mainNamespace = _main.attr("__dict__");

    //import needed module
    std::stringstream module;
    module << "import qi\n"
           << "from qicorecompat import *\n"
           << "import time\n"
           << "import random\n"
           << "from naoqi import *\n";

    try {
      py::exec(py::str(module.str().c_str()), _mainNamespace);
    }
    catch(py::error_already_set const&)
    {
      PyErr_Print();
    }

    //ALBroker
    std::stringstream albroker;
    albroker << "global al\n"
             << "al = ALBroker('PythonBroker', '0.0.0.0', 9600, '" << ip << "', " << port << ")\n\n";

    try {
      py::exec(py::str(albroker.str().c_str()), _mainNamespace);
    }
    catch(py::error_already_set const&)
    {
      PyErr_Print();
    }

    std::stringstream initalframemanger;
    initalframemanger << "ALFrameManager.setBehaviorPath('" << dir << "')\n";
    try {
      py::exec(py::str(initalframemanger.str().c_str()), _mainNamespace);
    }
    catch(py::error_already_set const&)
    {
      PyErr_Print();
    }

    try {
      py::exec(py::str(std::string("global ALLeds\nALLeds = ALProxy('ALLeds')\n")), _mainNamespace);
    }
    catch(py::error_already_set const&)
    {
      PyErr_Print();
    }
  }

  void PythonBoxLoader::terminate()
  {
    {
      py::GILScopedLock lock;
      std::stringstream delalbroker;
      delalbroker <<
        "al.shutdown()\n"
        "del al\n";

      try {
        py::exec(py::str(delalbroker.str().c_str()), _mainNamespace);
      }
      catch(py::error_already_set const&)
      {
        PyErr_Print();
      }
    }

    PyEval_AcquireLock();
    PyThreadState_Swap(_mainThread);
    Py_Finalize();
  }

  PyInterpreterState *PythonBoxLoader::getInterpreter()
  {
    return _mainThread->interp;
  }

  bool PythonBoxLoader::registerPythonClass(BoxInstanceModelPtr instance)
  {
    py::GILScopedLock lock;
    std::string code = generatedClass(instance);

    if(instance->uid() == "root_1_Diagnostic_20_init_Bumpers_2")
      std::cout << code;
    //Execute generated code
    try {
      py::exec(py::str(code.c_str()), _mainNamespace);
    }
    catch(py::error_already_set const&)
    {
      PyErr_Print();
      return false;
    }

    //Replace GeneratedClass by real name
    ContentModelPtr pyContent = instance->interface()->contents()->findContent(ContentModel::ContentType_PythonScript);
    std::string myclass;
    bool pyfile = false;
    if(pyContent)
    {
      std::string pythonFileName = pyContent->path();
      std::string filepath(std::string(instance->behaviorPath() + "/" + pythonFileName));
      std::ifstream file(filepath.c_str());
      std::stringstream buffer;
      buffer << file.rdbuf();
      file.close();
      myclass = buffer.str();

      //If py file is not empty
      if(boost::algorithm::contains(myclass, "MyClass")) {
        pyfile = true;
        boost::replace_all(myclass, "GeneratedClass", "GeneratedClass_" + instance->uid());
      }
    }

    //Execute py file
    try {
      if(pyfile)
      {
        py::exec(py::str(myclass.c_str()), _mainNamespace);
      }
    }
    catch(py::error_already_set const&)
    {
      PyErr_Print();
      return false;
    }

    std::string unique_id = instance->uid();
    std::string script;

    if(pyfile)
    {
      script = std::string("qi.registerObjectFactory('")
        + unique_id
        + std::string("', MyClass)\n");
    }
    else
    {
      script = std::string("qi.registerObjectFactory('")
        + unique_id
        + std::string("', GeneratedClass_") + unique_id + std::string(")\n");
    }

    qiLogDebug() << "Srcipt register : " + script;
    py::str ex(script);

    try {
      py::exec(ex, _mainNamespace);
    }
    catch(py::error_already_set const&)
    {
      PyErr_Print();
      return false;
    }

    return true;
  }
}
