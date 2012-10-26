#pragma once

#ifndef _LIB_SCRIPTWRAPPER_SRC_INAOQI_H_
#define _LIB_SCRIPTWRAPPER_SRC_INAOQI_H_


#include <alpythontools/alpython.h>
#include <alerror/alerror.h>
#include <alvalue/alvalue.h>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace AL
{
  class ALBroker;
  class ALProxy;
  class ALModuleCore;
  class ALBehavior;
  class ALTimeline;

  /**
  * All classes visible in python are here
  */

  /**
  * broker visible in python
  */
  class broker
  {
  public:
    /**
    * constructor
    */
    broker( char *strName, char *strIP, int port, char *parentIP, int parentPort );
    ~broker();
    void shutdown();
    bool isModulePresent(char *pModule);
    ALValue getGlobalModuleList();
    boost::shared_ptr<ALBroker> getBroker();

  protected:
    boost::shared_ptr<ALBroker> fMyBroker;
  };


  /**
  * module visible in python (abstract class)
  * We inherit from it in module and behavior, to avoid code duplication.
  */
  class baseModule
  {
  public:
    baseModule();

    void BIND_PYTHON(char * module, char *method);
    void _bindWithParam(char * module, char *method, int paramNumber );

    void exit( void);
    std::string getName();

    // this method is used by proxies
    std::string getBrokerName();

    void setModuleDescription(char *pDescription);

    void addParam(char *pParam);

    void functionName(char *method, char *module, char *description);

    /**
     * \brief Bind a method on ALModule::_methodMissing.
     *        Used by python interpreter
     * \param pModule python module name
     * \param pMethod method
     * \param paramNumber parameters' number
     */
    void autoBind(const std::string &pModule,
                  const std::string &pMethod,
                  int paramNumber);

    /**
     * Should always raise an exception.
     * To bind Python methods, we bind empty C++ functions,
     * so they can appear on the web page.
     * But if we call them directly, it won't work.
     */
    std::string _methodMissing0() {return "";}
    std::string _methodMissing1(const int &) {return "";}
    std::string _methodMissing2(const int &, const int &) {return "";}
    std::string _methodMissing3(const int &, const int &, const int &) {return "";}
    std::string _methodMissing4(const int &, const int &, const int &, const int &) {return "";}
    std::string _methodMissing5(const int &, const int &, const int &, const int &, const int &) {return "";}
    std::string _methodMissing6(const int &, const int &, const int &, const int &, const int &, const int &) {return "";}


    std::string _methodMissing();
    std::string version();

  protected:
    boost::shared_ptr<ALModuleCore> fModule;
  };

  /**
  * python module
  */
  class module: public baseModule
  {
  public:
    module(char *name);

    // we have to redefine all basemodule methods because python seems to be stupid and is not able to call them otherwise.
    void BIND_PYTHON(char * module, char *method)
    {
      baseModule::BIND_PYTHON(module, method);
    }
    void exit( void)
    {
      baseModule::exit();
    }
    std::string getName()
    {
      return  baseModule::getName();
    }
    std::string getBrokerName()
    {
      return  baseModule::getBrokerName();
    }
    std::string _methodMissing()
    {
      return  baseModule::_methodMissing();
    }
    std::string version (  )
    {
      return  baseModule::version();
    }
  };

  class timeline // see ALTimeline for documentation
  {
  public :
    timeline(boost::shared_ptr<AL::ALTimeline> timeline);
    virtual ~timeline(){}
    void play(void);
    void pause(void);
    void stop(void);
    void goTo(int pFrame);
    void goTo(char* pFrameName);
    int getSize();
    int getFPS();
    void setFPS(const int fps);

  private:
    boost::weak_ptr<AL::ALTimeline> fTimeline;

  };

  /**
  * python behavior
  */
  class behavior : public baseModule
  {
  public :

    /**
    * Instanciate behavior
    */
    behavior(boost::shared_ptr<ALBroker> broker, std::string name);

    virtual ~behavior();

    /// deprecated since 1.14
    void setEnabled(bool pEnabled);
    /// deprecated since 1.14
    bool isEnabled();

    /**
    * setResources
    * @param pList: resource name list to store
    */
    /// deprecated since 1.14
    void setResources(const std::vector<std::string>& pList, int pTimeout);

    /// deprecated since 1.14
    void waitFor(const std::vector<std::string>& pList , const char *pCallback , int pTimeout, int pOwnerType);

    void waitResourcesCallback(const char *pCallback);

    // true if resource free
    bool isResourceFree(const std::vector<std::string>& pList);

    // we know the resources and wait it but not take it
    void waitResourceFree();

    // we know the resources and wait it or take it if possible
    void waitResources();

    void releaseResource();

    bool addInput( char *pInputName );

    bool addOutput( char *pOutputName, bool pIsBang );

    void addParameter( char * pParameterName, PyObject* pValue, bool pInheritFromParent );

    ALValue getParameter(char * pParameterName);
    ALValue getParametersList();
    void setParameter(char * pParameterName, PyObject* pValue);

    void connectInput(char* pInputName, char* pALMemoryValueName, bool enabled);
    void connectOutput(char* pInputName, char* pALMemoryValueName, bool enabled);
    void connectParameter(char* pInputName, char* pALMemoryValueName, bool enabled);

    void stimulateIO( char *pIOName, PyObject* pValue);

    // we have to redefine all basemodule methods because python seems to be stupid and is not able to call them otherwise.
    void BIND_PYTHON(char * module, char *method);

    /// deprecated since 1.14
    void setModuleDescription(char* description);

    /// deprecated since 1.14
    void addParam(char* description);

    /// deprecated since 1.14
    void exit( void);

    std::string getName();

    /// deprecated since 1.14
    std::string getBrokerName();

    std::string _methodMissing();

    /// deprecated since 1.14
    std::string version (  );

    bool hasTimeline();
    timeline getTimeline();
    bool hasParentTimeline();
    timeline getParentTimeline();

    void executePython(const std::string& pMethod, const AL::ALValue& pParams, AL::ALValue& pResult);

  protected:
    boost::shared_ptr<ALBehavior> fBehavior;
  };

  /**
  * Python proxy
  * Does not inherit from ALProxy, just get a proxy from current broker.
  */
  class proxy
  {
  public:
    proxy(char *name);
    proxy(char *name, bool reinstanciate);
    proxy(char *name, char *IP, int port);

    ALValue pythonCall(PyObject *param);
    int pythonPCall(PyObject *param);
    bool wait(int id, int timeout);
    void stop(int id);
    bool isRunning(int id);
  protected:
    // not const proxy, as we want to test fBroker before instanciating anything
    boost::shared_ptr<ALProxy> fProxy;
  };

} // namespace AL
#endif  // _LIB_SCRIPTWRAPPER_SRC_INAOQI_H_
