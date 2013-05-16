/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <qi/log.hpp>

#include <qicore-compat/transition.hpp>
#include <qicore-compat/box.hpp>
#include "box_p.hpp"
#include "transition_p.hpp"

namespace qi
{

BoxPrivate::BoxPrivate(Box* parent)
  : _parent (parent),
    _stateMachine (0),
    _timeline (0),
    _name ("Unnamed-Box"),
    _path ("./"),
    _onLoadCallback(0),
    _onUnloadCallback(0),
    _transitions(),
    _labels(),
    _begin(-1),
    _end(-1)
{
}

BoxPrivate::~BoxPrivate()
{
}

void BoxPrivate::load()
{
  _onLoadCallback();
}

void BoxPrivate::unload()
{
  _onUnloadCallback();
}

void BoxPrivate::registerOnLoadCallback(PyObject* callable)
{
  _onLoadCallback.assignCallback(callable);
}

void BoxPrivate::registerOnUnloadCallback(PyObject* callable)
{
  _onUnloadCallback.assignCallback(callable);
}

void BoxPrivate::addTransition(Transition *tr)
{
  tr->_p->setFromState(_parent);
  _transitions.push_back(tr);
}

void BoxPrivate::removeTransition(Transition *tr)
{
  _transitions.remove(tr);
  tr->_p->setFromState(0);
}

/* Public Class */
Box::Box()
  : _p (new BoxPrivate(this))
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

void Box::addTransition(Transition *tr)
{
  _p->addTransition(tr);
}

void Box::removeTransition(Transition *tr)
{
  _p->removeTransition(tr);
}

std::list<Transition*>& Box::getTransitions() const
{
  return _p->_transitions;
}

void Box::addLabel(std::string label)
{
  _p->_labels.push_back(label);
}

void Box::setInterval(int a, int b)
{
  _p->_begin = a;
  _p->_end = b;
}

const std::vector<std::string>& Box::getLabels() const
{
  return _p->_labels;
}

int Box::getIntervalBegin()
{
  return _p->_begin;
}

int Box::getIntervalEnd()
{
  return _p->_end;
}

};
