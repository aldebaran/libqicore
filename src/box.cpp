/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <newframemanager/box.h>
#include "box_private.h"

BoxPrivate::BoxPrivate(boost::shared_ptr<AL::ALBroker> broker, std::string name)
  : AL::behavior(broker, name),
    _timeline (0),
    _stateMachine (0)
{
}

BoxPrivate::~BoxPrivate()
{
}

/* Public Class */
Box::Box(boost::shared_ptr<AL::ALBroker> broker, std::string name)
  : _p (new BoxPrivate(broker, name))
{
}

Box::~Box()
{
  delete _p;
}

void Box::waitResourcesCallback(const char* pCallback)
{
  _p->waitResourcesCallback(pCallback);
}

bool Box::isResourceFree(const std::vector<std::string>& pList)
{
  return _p->isResourceFree(pList);
}

void Box::waitResourceFree()
{
  _p->waitResourceFree();
}

void Box::waitResources()
{
  _p->waitResources();
}

void Box::releaseResource()
{
  _p->releaseResource();
}

bool Box::addInput(char* pInputName)
{
  return _p->addInput(pInputName);
}

bool Box::addOutput(char* pOutputName, bool pIsBang)
{
  return _p->addOutput(pOutputName, pIsBang);
}

void Box::addParameter(char* pParameterName, PyObject* pValue, bool pInheritFromParent)
{
  _p->addParameter(pParameterName, pValue, pInheritFromParent);
}

AL::ALValue Box::getParameter(char* pParameterName)
{
  return _p->getParameter(pParameterName);
}

AL::ALValue Box::getParametersList()
{
  return _p->getParametersList();
}

void Box::setParameter(char* pParameterName, PyObject* pValue)
{
  return _p->setParameter(pParameterName, pValue);
}

void Box::connectInput(char* pInputName, char* pALMemoryValueName, bool enabled)
{
  _p->connectInput(pInputName, pALMemoryValueName, enabled);
}

void Box::connectOutput(char* pInputName, char* pALMemoryValueName, bool enabled)
{
  _p->connectOutput(pInputName, pALMemoryValueName, enabled);
}

void Box::connectParameter(char* pInputName, char* pALMemoryValueName, bool enabled)
{
  _p->connectParameter(pInputName, pALMemoryValueName, enabled);
}

void Box::stimulateIO(char* pIOName, PyObject* pValue)
{
  _p->stimulateIO(pIOName, pValue);
}

void Box::BIND_PYTHON(char* module, char *method)
{
  _p->BIND_PYTHON(module, method);
}

std::string Box::getName()
{
  return _p->getName();
}

std::string Box::_methodMissing()
{
  return _p->_methodMissing();
}

void Box::functionName(char *method, char *module, char *description)
{
  _p->functionName(method, module, description);
}

void Box::addParam(char *pParam)
{
  _p->addParam(pParam);
}

void Box::_bindWithParam(char *module, char *method, int paramNumber)
{
  _p->_bindWithParam(module, method, paramNumber);
}

int Box::getTimelineFps(const std::string &pId)
{
  /* TODO: Use pId parameter here */
  if (!_p->_timeline)
  /* FIXME ... Should not happend in legacy framemanager */
    return 25;
  return _p->_timeline->getFPS();
}

void Box::setTimelineFps(const std::string &pId, const int &pFPS)
{
  /* TODO: Use pId parameter here */
  if (_p->_timeline)
    _p->_timeline->setFPS(pFPS);
}

Timeline* Box::getTimeline()
{
  return _p->_timeline;
}

void Box::setTimeline(Timeline* t)
{
  _p->_timeline = t;
}

bool Box::hasTimeline() const
{
  return _p->_timeline;
}

StateMachine* Box::getStateMachine()
{
  return _p->_stateMachine;
}

void Box::setStateMachine(StateMachine* s)
{
  _p->_stateMachine = s;
}

bool Box::hasStateMachine() const
{
  return _p->_stateMachine;
}

void Box::executePython(const std::string &pMethod, const AL::ALValue &pParams, AL::ALValue &pResult)
{
  _p->executePython(pMethod, pParams, pResult);
}
