/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <newframemanager/state_machine.h>
#include <newframemanager/transition.h>

#include "transition_private.h"

TransitionPrivate::TransitionPrivate(Transition* parent, State* toState)
  : _hasTimeOut (false),
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

void TransitionPrivate::setFromState(State *s)
{
  _fromState = s;
}

State* TransitionPrivate::getFromState()
{
  return _fromState;
}

State* TransitionPrivate::getToState()
{
  return _toState;
}

/* -- Public -- */

Transition::Transition(State* s)
  : _p (new TransitionPrivate(this, s))
{
}

Transition::~Transition()
{
  delete _p;
}

void Transition::load(StateMachine *s)
{
  _p->setMachine(s);
}

void Transition::unload()
{
  _p->setMachine(0);
}

void Transition::trigger()
{
  _p->triggerTransition();
}

State* Transition::getFromState()
{
  return _p->getFromState();
}

State* Transition::getToState()
{
  return _p->getToState();
}

bool Transition::hasTimeOut()
{
  return _p->_hasTimeOut;
}

int Transition::getTimeOut()
{
  return _p->_timeOut;
}

void Transition::setTimeOut(int n)
{
  _p->_timeOut = n;
  _p->_hasTimeOut = true;
}
