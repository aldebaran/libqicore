#include <alpythontools/alpythontools.h>
#include <alcommon/albrokermanager.h>
#include <alcommon/almodule.h>
#include <alerror/alerror.h>

#include <alproxies/almemoryproxy.h>
#include <albehavior/altimeline.h>
#include <alcommon/albroker.h>
#include <althread/almutex.h>
#include <althread/alcriticalsectionwrite.h>
#include <alcommon/alproxy.h>
#include <alproxies/almemoryproxy.h>
#include <alpythontools/swigpyrun.h>
#include <altools/tools.h>
#include <qicore/newinaoqi.h>
#include "newinaoqi_private.h"

#include <iostream>

#include <qi/log.hpp>

using namespace AL;

#ifndef SWIG_as_voidptr
#define SWIG_as_voidptr(a) const_cast< void * >(static_cast< const void * >(a))
#endif

// we have this global variable here
// people in python does not have to use a broker as parameter when creating
// a proxy/module/behavior, for convenience.
// Therefore the broker's complexity is masked.
// When creating a Proxy in python with IP&port, it will call proxy::createEmptyBroker
// if necessary (only if there is no current broker).
boost::weak_ptr<AL::ALBroker> fBroker;


/*
template <class T>
static boost::shared_ptr<T> createPythonModule(boost::shared_ptr<ALBroker> pBroker, const std::string& name)
{
  boost::shared_ptr<T> module = createModule<T>(pBroker, name);
  module->fModuleType = AL::ALModule::PYTHON;
  return module;
}*/


template <class T>
static boost::shared_ptr<T> createPythonModuleCore(boost::shared_ptr<ALBroker> pBroker, const std::string& name)
{
  boost::shared_ptr<T> module = AL::ALModuleCore::createModuleCore<T>(pBroker, name);
  module->setModuleType(AL::ALModuleCore::PYTHON);
  return module;
}



void setInstance(long pointerBroker)
{
  boost::shared_ptr<AL::ALBroker>* testBroker = (boost::shared_ptr<AL::ALBroker> *) ( pointerBroker);
  boost::shared_ptr<AL::ALBroker> lBroker = *testBroker;
  AL::ALBrokerManager::setInstance(lBroker->fBrokerManager.lock());
  AL::ALBrokerManager::getInstance()->addBroker(lBroker);
  fBroker = lBroker;
}

// already deprecated
bool _ALSystem(const char *pCommand)
{
    const char *sourceCommand[20];
    std::string command = std::string(pCommand);
    std::istringstream iss(command);
    std::vector<std::string> tokens;

    copy(std::istream_iterator<std::string>(iss),
               std::istream_iterator<std::string>(),
               std::back_inserter<std::vector<std::string> >(tokens));

    if (tokens.size()>20)
    {
      qiLogError("behavior.inaoqi") << "Too much parameters in command" << std::endl;
      return false;
    }

    for(unsigned int i = 0 ; i < tokens.size() ; i++)
    {
      sourceCommand[i] = tokens[i].c_str();
    }
    sourceCommand[tokens.size()] = NULL;


    return ALTools::LaunchShellCommand(sourceCommand,true); // aaze
}


broker::broker( char *strName, char *strIP, int port, char *parentIP, int parentPort )
{
  if(strName == NULL || strIP == NULL || parentIP == NULL)
    throw ALERROR("broker", "broker", "NULL values were passed as arguments");

  // EPOT 10/12/2008
  // Ok, you may wonder : "why on earth would we initialize python threads here ?"
  // Why ? Because otherwise, remote python script crashes on Mac when releasing a GilState.
  // So yes, we could have rejected Mac because they are ugly and they have an apple key. But they also have photo booth, so its hard to say no...
  // All in all, it seems safe to use this method more than needed, so we do not even need a ifdef CRAPPY_MAC.
  PyEval_InitThreads();

  std::cout << "inaoqi-broker: construct at " << std::string(strIP) << ":" << port << ", parent : " << std::string(parentIP) << ":" << parentPort << std::endl;

  // do not load network on this lib, as on Linux it does not work
  // that is why we recompile Network code here in this module...
  //fBroker = AL::ALBroker::createBroker(strName, strIP, port, parentIP, parentPort);
  boost::shared_ptr<AL::ALBroker> lBroker = AL::ALBroker::createBroker(strName, strIP, port, parentIP, parentPort, true);
  fBroker = lBroker;
}

bool broker::isModulePresent(char *pModule)
{
  boost::shared_ptr<AL::ALBroker> lBroker = fBroker.lock();
  if(!lBroker)
    return false;
  return (lBroker->isModulePresent(pModule));
}

broker::~broker(void)
{
  AL::ALBrokerManager::getInstance()->removeBroker(fMyBroker);
}

void broker::shutdown()
{
  boost::shared_ptr<AL::ALBroker> lBroker = fBroker.lock();
  if(!lBroker)
    return;

  boost::shared_ptr<ALModuleInfo> info(new (ALModuleInfo));
  info->name = lBroker->getName();
  qiLogInfo("behavior.inaoqi") << "Stop broker "  << info->name << std::endl;
  lBroker->shutdown();
}

AL::ALValue broker::getGlobalModuleList(void)
{
  boost::shared_ptr<AL::ALBroker> lBroker = fBroker.lock();
  AL::ALValue res;
  if(lBroker)
  {
    boost::shared_ptr <std::vector<ALModuleInfo> > pModuleList (new std::vector<ALModuleInfo> );
    lBroker->getGlobalModuleList(pModuleList);
    for( std::vector<ALModuleInfo>::iterator it = pModuleList->begin(), itEnd = pModuleList->end(); it != itEnd; ++it )
    {
        AL::ALValue moduleInfo;
        moduleInfo.arrayPush((*it).name.c_str());
        res.arrayPush(moduleInfo);
    }
  }
  else
  {
    printf("no broker found");
  }
  fflush(NULL);
  return res;
}

boost::shared_ptr<ALBroker> broker::getBroker()
{
  boost::shared_ptr<AL::ALBroker> lBroker = fBroker.lock();
  return lBroker;
}

baseModule::baseModule()
{
}

void baseModule::autoBind(const std::string &module,
                          const std::string &method,
                          int paramNumber)
{
  if (paramNumber == -1)
  {
    fModule->functionName(method, module, "python method");
    fModule->addParam("empty param", "avoid exception");
    fModule->getCurrentMethodDescription().mask = 0;
    completeAndCheck(&baseModule::_methodMissing0, fModule->getCurrentMethodDescription());
    fModule->bindMethod(createFunctor(this, &baseModule::_methodMissing0));
    return;
  }

  switch (paramNumber)
  {
  case 0  :
    completeAndCheck(&baseModule::_methodMissing0, fModule->getCurrentMethodDescription());
    fModule->bindMethod(createFunctor(this, &baseModule::_methodMissing0));
    break;
  case 1  :
    completeAndCheck(&baseModule::_methodMissing1, fModule->getCurrentMethodDescription());
    fModule->bindMethod(createFunctor(this, &baseModule::_methodMissing1));
    break;
  case 2  :
    completeAndCheck(&baseModule::_methodMissing2, fModule->getCurrentMethodDescription());
    fModule->bindMethod(createFunctor(this, &baseModule::_methodMissing2));
    break;
  case 3  :
    completeAndCheck(&baseModule::_methodMissing3, fModule->getCurrentMethodDescription());
    fModule->bindMethod(createFunctor(this, &baseModule::_methodMissing3));
    break;
  case 4  :
    completeAndCheck(&baseModule::_methodMissing4, fModule->getCurrentMethodDescription());
    fModule->bindMethod(createFunctor(this, &baseModule::_methodMissing4));
    break;
  case 5  :
    completeAndCheck(&baseModule::_methodMissing5, fModule->getCurrentMethodDescription());
    fModule->bindMethod(createFunctor(this, &baseModule::_methodMissing5));
    break;
  case 6  :
    completeAndCheck(&baseModule::_methodMissing6, fModule->getCurrentMethodDescription());
    fModule->bindMethod(createFunctor(this, &baseModule::_methodMissing6));
    break;
  default:
    completeAndCheck(&baseModule::_methodMissing0, fModule->getCurrentMethodDescription());
    fModule->bindMethod(createFunctor(this, &baseModule::_methodMissing0));
    break;
  }
}

void baseModule::BIND_PYTHON(char * module, char *method)
{
  if(module == NULL || method == NULL)
    throw ALERROR("baseModule", "BIND_PYTHON", "NULL values were passed as arguments");

  assert(fModule);
  ALCriticalSectionAllowThreadsPython _section;
  autoBind(module, method,-1);
}

void baseModule::_bindWithParam(char * module, char *method, int paramNumber)
{
  assert(fModule);
  ALCriticalSectionAllowThreadsPython _section;
  autoBind(module, method,paramNumber);
}

void baseModule::functionName(char *method, char *module, char *description)
{
  if(method == NULL || module == NULL || description == NULL)
    throw ALERROR("baseModule", "functionName", "NULL values were passed as arguments");

  ALCriticalSectionAllowThreadsPython _section;
  fModule->functionName(method, module, description);
  // no option isCpp
  fModule->getCurrentMethodDescription().mask = 0;
}

void baseModule::setModuleDescription(char * pDescription)
{
  if(pDescription == NULL)
    fModule->setModuleDescription("");
  else
    fModule->setModuleDescription(pDescription);
}

void baseModule::addParam(char * param)
{
  if(param == NULL)
    throw ALERROR("baseModule", "addParam", "NULL values were passed as arguments");

  ALCriticalSectionAllowThreadsPython _section;
  fModule->addParam(param, "Python's parameter");
}


void baseModule::exit( void)
{
  assert(fModule);
  ALCriticalSectionAllowThreadsPython _section;
  fModule->exit();
}

std::string baseModule::getName()
{
  assert(fModule);
  ALCriticalSectionAllowThreadsPython _section;
  return (fModule->getName());
}
std::string baseModule::getBrokerName()
{
  assert(fModule);
  ALCriticalSectionAllowThreadsPython _section;
  return (fModule->getBrokerName());
}


std::string baseModule::_methodMissing()
{
  printf("\nreceive method missing\n");
  throw ALERROR("module(python)", "methodMissing", "Function does not exist or not implemented");
}

std::string baseModule::version()
{
  return "PythonModule";
}

ALMethodInfo* ALPythonModule::execute(const std::string& pMethod, const AL::ALValue& pParams, AL::ALValue& pResult)
{
  ALMethodInfo methDesc = getMethodHelpObject(pMethod);
  if(methDesc.isCpp()) //method is a C++ one !
  {
    return ALModuleCore::execute(pMethod, pParams, pResult);
  }

  executePython(getName(), pMethod, pParams, pResult);

  return NULL; // no way this is an asynchronous task in python, so just return NULL at the moment.
}

void ALPythonModule::executePython(const std::string& pModuleName, const std::string& pMethod, const AL::ALValue& pParams, AL::ALValue& pResult)
{
  std::cout << "executePython with moduleName: " << pModuleName << " method: " << pMethod << std::endl;
  std::string err = AL::ALPythonTools::eval(pModuleName, pMethod, pParams, pResult);
  if(err != "")
  {
    throw ALERROR("ALPythonModule", "execute", err);
  }
}

ALPythonBehavior::ALPythonBehavior(boost::shared_ptr<AL::ALBroker> pBroker, const std::string& name)
  : AL::ALBehavior(pBroker, name)
{
}

ALMethodInfo* ALPythonBehavior::execute(const std::string& pMethod, const AL::ALValue& pParams, AL::ALValue& pResult)
{
  ALMethodInfo methDesc = getMethodHelpObject(pMethod);
  if(methDesc.isCpp()) //method is a C++ one !
  {
    return ALModuleCore::execute(pMethod, pParams, pResult);
  }

  /* We take the object not the class */
  ALPythonModule::executePython(getName() + "_obj", pMethod, pParams, pResult);
  return NULL; // no way this is an asynchronous task in python, so just return NULL at the moment.
}


module::module(char *name)
  : baseModule()
{

  boost::shared_ptr<AL::ALBroker> lBroker = fBroker.lock();
  if(lBroker)
  if(name == NULL)
    throw ALERROR("module", "module", "NULL value was passed as argument");
  if(lBroker == NULL)
    throw ALERROR("module", "module", "Could not create a module, as there is no current broker in Python's world.");

  ALCriticalSectionAllowThreadsPython _section;
  fModule = createPythonModuleCore<ALPythonModule>(lBroker, name);
}

// see ALTimeline for documentation

timeline::timeline(boost::shared_ptr<AL::ALTimeline> timeline)
  : fTimeline(timeline)
{
}
void timeline::play(void)
{
  ALCriticalSectionAllowThreadsPython _section;

  if(boost::shared_ptr<ALTimeline> t = fTimeline.lock())
    t->play();
}

void timeline::pause(void)
{
  ALCriticalSectionAllowThreadsPython _section;
  if(boost::shared_ptr<ALTimeline> t = fTimeline.lock())
    t->pause();
}
void timeline::stop(void)
{
  ALCriticalSectionAllowThreadsPython _section;
  if(boost::shared_ptr<ALTimeline> t = fTimeline.lock())
    t->stop();
}

void timeline::goTo(int pFrame)
{
  ALCriticalSectionAllowThreadsPython _section;
  if(boost::shared_ptr<ALTimeline> t = fTimeline.lock())
    t->goTo(pFrame);
}

void timeline::goTo(char* pFrameName)
{
  ALCriticalSectionAllowThreadsPython _section;
  if(boost::shared_ptr<ALTimeline> t = fTimeline.lock())
    t->goTo(pFrameName);
}
int timeline::getSize()
{
  ALCriticalSectionAllowThreadsPython _section;
  if(boost::shared_ptr<ALTimeline> t = fTimeline.lock())
    return t->getSize();
  return 0;
}

int timeline::getFPS()
{
  ALCriticalSectionAllowThreadsPython _section;
  if(boost::shared_ptr<ALTimeline> t = fTimeline.lock())
    return t->getFPS();
  return 0;
}

void timeline::setFPS(const int fps)
{
  ALCriticalSectionAllowThreadsPython _section;
  if(boost::shared_ptr<ALTimeline> t = fTimeline.lock())
    t->setFPS(fps);
}



behavior::behavior(boost::shared_ptr<ALBroker> broker, std::string name)
  : baseModule()
{
  ALCriticalSectionAllowThreadsPython _section;
  fBehavior = createPythonModuleCore<ALPythonBehavior>(broker, name);

  fModule = fBehavior;
}

behavior::~behavior()
{
  baseModule::exit();
}


void behavior::setEnabled(bool pEnabled)
{
  qiLogWarning("inaoqi") << "behavior::setEnabled" << "(" << pEnabled << ")" <<" is deprecated ! ";
  fBehavior->setEnabled(pEnabled);
}
bool behavior::isEnabled()
{
  qiLogWarning("inaoqi") << "behavior::isEnabled is deprecated.";
  return fBehavior->isEnabled();
}


/**
* setResources
* @param pList: resource name list to store
*/
void behavior::setResources(const std::vector<std::string>& pList, int pTimeout)
{
  qiLogWarning("inaoqi") << "behavior::setResources is deprecated. You should use Choregraphe to set Resources";
  ALCriticalSectionAllowThreadsPython _section;
  fBehavior->AL::ALBehavior::setResources(pList, pTimeout);
}

void behavior::waitFor(const std::vector<std::string>& pList , const char *pCallback , int pTimeout, int pOwnerType)
{
  ALCriticalSectionAllowThreadsPython _section;
  fBehavior->AL::ALBehavior::waitFor(pList, pCallback,pTimeout,pOwnerType  );
}

void behavior::waitResourcesCallback(const char *pCallback)
{
  ALCriticalSectionAllowThreadsPython _section;
  fBehavior->AL::ALBehavior::waitResourcesCallback(pCallback);
}

// true if resource free
bool behavior::isResourceFree(const std::vector<std::string>& pList)
{
  ALCriticalSectionAllowThreadsPython _section;
  return fBehavior->AL::ALBehavior::isResourceFree(pList);
}

// we know the resources and wait it but not take it
void behavior::waitResourceFree()
{
  ALCriticalSectionAllowThreadsPython _section;
  fBehavior->AL::ALBehavior::waitResourceFree();
}

// we know the resources and wait it or take it if possible
void behavior::waitResources()
{
  ALCriticalSectionAllowThreadsPython _section;
  return fBehavior->AL::ALBehavior::waitResources();
}

void behavior::releaseResource()
{
  ALCriticalSectionAllowThreadsPython _section;
  fBehavior->AL::ALBehavior::releaseResource();
}

bool behavior::addInput( char *pInputName )
{
  return (fBehavior->ALBehavior::addInput(pInputName));
}

bool behavior::addOutput( char *pOutputName, bool pIsBang )
{
  return (fBehavior->addOutput(pOutputName, pIsBang));
}
void behavior::addParameter( char * pParameterName, PyObject* pValue, bool pInheritFromParent )
{
  return (fBehavior->addParameter(pParameterName, ALPythonTools::convertPyObjectToALValue(pValue), pInheritFromParent ));
}
ALValue behavior::getParameter(char * pParameterName)
{
  return (fBehavior->getParameter(pParameterName));
}
ALValue behavior::getParametersList()
{
  return (fBehavior->getParametersList());
}
void behavior::setParameter(char * pParameterName, PyObject* pValue)
{
  return (fBehavior->setParameter(pParameterName, ALPythonTools::convertPyObjectToALValue(pValue)));
}

void behavior::stimulateIO( char *pIOName, PyObject* pValue)
{
  fBehavior->stimulateIO(pIOName,ALPythonTools::convertPyObjectToALValue(pValue));
}


// we have to redefine all basemodule methods because python seems to be stupid and is not able to call them otherwise.
void behavior::BIND_PYTHON(char * module, char *method)
{
  baseModule::BIND_PYTHON(module, method);
}

void behavior::setModuleDescription(char* description)
{
  baseModule::setModuleDescription(description);
}

void behavior::addParam(char* description)
{
  baseModule::addParam(description);
}

void behavior::exit( void)
{
  qiLogWarning("inaoqi") << "behavior::exit is deprecated. You should call ALBehaviorManager.";
  baseModule::exit();
}
std::string behavior::getName()
{
  return  baseModule::getName();
}
std::string behavior::getBrokerName()
{
  qiLogWarning("inaoqi") << "behavior::getBrokerName is deprecated.";
  return  baseModule::getBrokerName();
}

bool behavior::hasTimeline()
{
  return fBehavior->getTimeline() != NULL;
}
timeline behavior::getTimeline()
{
  return timeline(fBehavior->getTimeline());
}

bool behavior::hasParentTimeline()
{
  return fBehavior->getParentTimeline() != NULL;
}
timeline behavior::getParentTimeline()
{
  return timeline(fBehavior->getParentTimeline());
}

std::string behavior::_methodMissing()
{
  return  baseModule::_methodMissing();
}
std::string behavior::version (  )
{
  qiLogWarning("inaoqi") << "behavior::version is deprecated. You should call ALMemory.version()";
  return  baseModule::version();
}

void behavior::connectInput(char* pInputName, char* pALMemoryValueName, bool enabled)
{
  fBehavior->_connectInput(std::string(pInputName), std::string(pALMemoryValueName), enabled);
}

void behavior::connectOutput(char* pInputName, char* pALMemoryValueName, bool enabled)
{
  fBehavior->_connectOutput(std::string(pInputName), std::string(pALMemoryValueName), enabled);
}

void behavior::connectParameter(char* pInputName, char* pALMemoryValueName, bool enabled)
{
  fBehavior->_connectParameter(std::string(pInputName), std::string(pALMemoryValueName), enabled);
}

void behavior::executePython(const std::string& pMethod, const AL::ALValue& pParams, AL::ALValue& pResult)
{
  std::cout << "executePython in behavior with moduleName: " << getName() << " method: " << pMethod << std::endl;
  std::string err = AL::ALPythonTools::eval(getName() + "_obj", pMethod, pParams, pResult);
  if(err != "")
  {
    throw ALERROR("ALPythonModule", "execute", err);
  }
}

proxy::proxy(char *name)
{
  if(name == NULL)
    throw ALERROR("proxy", "proxy", "NULL value was passed as argument");
  boost::shared_ptr<AL::ALBroker> lBroker = fBroker.lock();
  if(lBroker == NULL)
    throw ALERROR("proxy", "proxy", "Could not create a proxy, as there is no current broker in Python's world.");

  fProxy = lBroker->getProxy(name);
}

proxy::proxy(char *name, bool reinstanciate)
{
  if(name == NULL)
    throw ALERROR("proxy", "proxy", "NULL value was passed as argument");
  boost::shared_ptr<AL::ALBroker> lBroker = fBroker.lock();
  if(lBroker == NULL)
    throw ALERROR("proxy", "proxy", "Could not create a proxy, as there is no current broker in Python's world.");

  if (reinstanciate)
  {
    fProxy = boost::shared_ptr<AL::ALProxy>(new AL::ALProxy(lBroker, name));
  }
  else
  {
    fProxy = lBroker->getProxy(name);
  }

}

proxy::proxy(char *name, char *IP, int port)
{
  if(name == NULL || IP == NULL)
    throw ALERROR("proxy", "proxy", "NULL values were passed as arguments");

  fProxy = boost::shared_ptr<AL::ALProxy>(new AL::ALProxy(name, IP, port));
}

AL::ALValue proxy::pythonCall(PyObject *param)
{
  AL::ALValue resultParam;
  bool methodFilled = false;
  std::string strMethod;
  bool isList = PyList_Check(param);
  if (isList)
  {
    int size = PyList_Size(param);
    // take all parameters at python format and make an ALValue
    for (int i = 0; i < size; ++i)
    {
      PyObject *a = PyList_GetItem(param, i);
      AL::ALValue value = ALPythonTools::convertPyObjectToALValue(a);

      if(methodFilled == false)
      {
        strMethod = std::string(value);
        methodFilled = true;
      }
      else
        resultParam.arrayPush(value);
    }
  }

  AL::ALValue r;
  bool raised = false;
  AL::ALError err;

  ALCriticalSectionAllowThreadsPython section;
  try
  {
    r = (fProxy->genericCall(strMethod, resultParam));
  }
  catch(AL::ALError& e)
  {
    raised = true;
    err = e;
  }

  if(raised)
    throw(err);

  return r;
}

int proxy::pythonPCall(PyObject *param)
{

    AL::ALValue resultParam;
  bool methodFilled = false;
  std::string strMethod;
  bool isList = PyList_Check(param);
  if (isList)
  {
    int size = PyList_Size(param);
    for (int i = 0; i < size; ++i)
    {
      PyObject *a = PyList_GetItem(param, i);
      AL::ALValue value = ALPythonTools::convertPyObjectToALValue(a);
      if(methodFilled == false)
      {
        strMethod = std::string(value);
        methodFilled = true;
      }
      else
        resultParam.arrayPush(value);
    }
  }

  int r;
  bool raised = false;
  AL::ALError err;

  ALCriticalSectionAllowThreadsPython section;
  try
  {
    r  = fProxy->genericPCall(strMethod, resultParam);
  }
  catch(AL::ALError& e)
  {
    raised = true;
    err = e;
  }

  if(raised)
    throw(err);
  return r;
}

bool proxy::wait(int id, int timeout)
{
  ALCriticalSectionAllowThreadsPython section;
  return fProxy->wait(id, timeout);
}

void proxy::stop(int id)
{
  ALCriticalSectionAllowThreadsPython section;
  return fProxy->stop(id);
}

bool proxy::isRunning(int id)
{
  ALCriticalSectionAllowThreadsPython section;
  return fProxy->isRunning(id);
}

#ifdef getMemoryProxy
#undef getMemoryProxy
#endif

AL::ALMemoryProxy getMemoryProxy()
{
  boost::shared_ptr<AL::ALBroker> lBroker = fBroker.lock();
  if(lBroker)
    return ALMemoryProxy(lBroker);
  return ALMemoryProxy();
}
