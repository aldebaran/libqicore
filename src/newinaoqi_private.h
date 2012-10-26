#pragma once

#ifndef _LIB_SCRIPTWRAPPER_SRC_INAOQI_PRIVATE_H_
#define _LIB_SCRIPTWRAPPER_SRC_INAOQI_PRIVATE_H_


#include <alerror/alerror.h>
#include <alvalue/alvalue.h>
#include <alcommon/almodule.h>
#include <albehavior/albehavior.h>
#include <alcommon/alproxy.h>
#include <boost/shared_ptr.hpp>

namespace AL
{
  class ALBroker;

  /**
  * Create an implementation of ALModule so a Python module can be created.
  * We use this implementModule because we don't want module to inherit directly from ALModule :
  * This way we can call createModule, and call init, initModule all the time.
  * We redefine the pure virtual methods here.
  */
  class ALPythonModule: public ALModule
  {
  public:
    ALPythonModule(boost::shared_ptr<ALBroker> pBroker, const std::string& name): ALModule(pBroker, name)
    {

    }
    std::string version (  )
    {
      return "unused";
    }

    /**
    * We redefine this method so we can call the correct method in there
    */
    virtual ALMethodInfo* execute(const std::string& pMethod, const ALValue& pParams, ALValue& pResult);

    bool innerTest( void )
    {
      return true;
    }

    // define this static method, as we also want ALBehaviorPython to use it.
    static void executePython(const std::string& pModuleName, const std::string& pMethod, const ALValue& pParams, ALValue& pResult);
  };

  /**
  * Create an implementation of ALBehavior so the correct execute is called
  *
  */
  class ALPythonBehavior: public ALBehavior
  {
  public:
    ALPythonBehavior(boost::shared_ptr<ALBroker> pBroker, const std::string& name);

    /**
    * We redefine this method so we can call the correct method in there
    */
    virtual ALMethodInfo* execute(const std::string& pMethod, const ALValue& pParams, ALValue& pResult);
  };

} // namespace AL
#endif  // _LIB_SCRIPTWRAPPER_SRC_INAOQI_H_
