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

#include <qicore-compat/model/boxinstancemodel.hpp>
#include <qicore-compat/model/boxinterfacemodel.hpp>
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

  void PythonBoxLoader::initPython(const std::string &ip, const std::string &port)
  {
    Py_Initialize();
    PyEval_InitThreads();

    //import python module
    char ppath[5];
    strcpy(ppath, "path");
    PyObject* sysPath = PySys_GetObject(ppath);

    std::string qisdkpython = boost::filesystem::path(qi::path::findLib("_qi")).parent_path().string();
    std::string naoqisdkpython = boost::filesystem::path(qi::path::findLib("_inaoqi")).parent_path().string();

    //TODO
    std::string qicorecompatpath = "/home/camille/work/master/lib/libqicore/libqicore-compat/python/";
    std::string qipath = "/home/camille/work/master/lib/qimessaging/python/";

    PyList_Insert(sysPath, 0, PyString_FromString(qipath.c_str()));
    PyList_Insert(sysPath, 0, PyString_FromString(qisdkpython.c_str()));
    PyList_Insert(sysPath, 0, PyString_FromString(naoqisdkpython.c_str()));
    PyList_Insert(sysPath, 0, PyString_FromString(qicorecompatpath.c_str()));

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
  }

  bool PythonBoxLoader::registerPythonClass(BoxInstanceModelPtr instance)
  {
    std::string code = generatedClass(instance);

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
    if(pyContent)
    {
      std::string pythonFileName = pyContent->path();
      std::string filepath(std::string(instance->behaviorPath() + "/" + pythonFileName));
      std::ifstream file(filepath.c_str());
      std::stringstream buffer;
      buffer << file.rdbuf();
      file.close();
      myclass = buffer.str();
      boost::replace_all(myclass, "GeneratedClass", "GeneratedClass_" + instance->uid());
    }

    //Execute py file
    try {
      if(pyContent)
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

    if(pyContent)
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
