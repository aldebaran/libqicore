/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <qicore/statemachine.hpp>
#include <qicore/transition.hpp>

#include "transition_private.hpp"

namespace qi
{

TransitionPrivate::TransitionPrivate(Transition* parent, Box* toState)
  : _name ("Unnamed-Transition"),
    _hasTimeOut (false),
    _timeOut (0),
    _machine (0),
    _toState (toState),
    _parent (parent)
{
}

TransitionPrivate::~TransitionPrivate()
{
}

void TransitionPrivate::triggerTransition()
{
  if (_machine)
    _machine->executeTransition(_parent);
}

void TransitionPrivate::setMachine(StateMachine *s)
{
  _machine = s;
}

void TransitionPrivate::setFromState(Box *s)
{
  _fromState = s;
}

Box* TransitionPrivate::getFromState()
{
  return _fromState;
}

Box* TransitionPrivate::getToState()
{
  return _toState;
}

void TransitionPrivate::load(StateMachine *s)
{
  setMachine(s);
}

void TransitionPrivate::unload()
{
  setMachine(0);
}

/* -- Public -- */

Transition::Transition(Box* s)
  : _p (new TransitionPrivate(this, s))
{
}

Transition::~Transition()
{
  delete _p;
}

void Transition::trigger()
{
  _p->triggerTransition();
}

Box* Transition::getFromState() const
{
  return _p->getFromState();
}

Box* Transition::getToState() const
{
  return _p->getToState();
}

bool Transition::hasTimeOut() const
{
  return _p->_hasTimeOut;
}

int Transition::getTimeOut() const
{
  return _p->_timeOut;
}

void Transition::setTimeOut(int n)
{
  _p->_timeOut = n;
  _p->_hasTimeOut = true;
}

void Transition::setName(std::string name)
{
  _p->_name = name;
}

std::string Transition::getName() const
{
  return _p->_name;
}

};
