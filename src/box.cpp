/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <qi/log.hpp>

#include <qicore/box.hpp>
#include "box_private.hpp"

namespace qi
{

BoxPrivate::BoxPrivate()
  : _stateMachine (0),
    _timeline (0),
    _name ("Unnamed-Box"),
    _path ("./"),
    _onLoadCallback(0),
    _onUnloadCallback(0)
{
}

BoxPrivate::~BoxPrivate()
{
}

void BoxPrivate::load()
{
  invokeCallback(_onLoadCallback);
}

void BoxPrivate::unload()
{
  invokeCallback(_onUnloadCallback);
}

void BoxPrivate::registerOnLoadCallback(PyObject* callable)
{
  Py_XDECREF(_onLoadCallback);
  _onLoadCallback = callable;
  Py_XINCREF(_onLoadCallback);
}

void BoxPrivate::registerOnUnloadCallback(PyObject* callable)
{
  Py_XDECREF(_onUnloadCallback);
  _onUnloadCallback = callable;
  Py_XINCREF(_onUnloadCallback);
}

void BoxPrivate::invokeCallback(PyObject* callback)
{
  if (!callback)
    return;

  PyObject* ret;
  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();

  ret = PyObject_CallFunctionObjArgs(callback, NULL);
  if (!ret)
  {
    qiLogError("qiCore.box") << "Unable to call python callback";
    PyErr_Print();
    PyErr_Clear();
  }

  Py_XDECREF(ret);

  PyGILState_Release(gstate);
}

/* Public Class */
Box::Box()
  : _p (new BoxPrivate())
{
}

Box::~Box()
{
  delete _p;
}

Timeline* Box::getTimeline() const
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

StateMachine* Box::getStateMachine() const
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

void Box::setName(std::string name)
{
  _p->_name = name;
}

std::string Box::getName() const
{
  return _p->_name;
}

void Box::setPath(std::string path)
{
  _p->_path = path;
}

std::string Box::getPath() const
{
  return _p->_path;
}

void Box::registerOnLoadCallback(PyObject* callable)
{
  _p->registerOnLoadCallback(callable);
}

void Box::registerOnUnloadCallback(PyObject* callable)
{
  _p->registerOnUnloadCallback(callable);
}

};
