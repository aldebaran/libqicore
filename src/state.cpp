/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <newframemanager/diagram.h>

#include "state_private.h"
#include "transition_private.h"
#include "state_machine_private.h"

StatePrivate::StatePrivate(State* parent)
  : _parent (parent),
    _diagram (0)
{
}

StatePrivate::~StatePrivate()
{
  for (std::list<Transition*>::iterator it = _transitions.begin();
        it != _transitions.end(); it++)
    delete (*it);
}

void StatePrivate::addTransition(Transition *tr)
{
  tr->_p->setFromState(_parent);
  _transitions.push_back(tr);
}

void StatePrivate::removeTransition(Transition *tr)
{
  _transitions.remove(tr);
  tr->_p->setFromState(0);
}

void StatePrivate::setDiagram(Diagram *d)
{
  _diagram = d;
}

void StatePrivate::onEnter()
{
}

void StatePrivate::onExit()
{
}

/* -- Public -- */

State::State()
  : _p (new StatePrivate(this))
{
}

State::~State()
{
  delete _p;
}

void State::onEnter()
{
  _p->onEnter();
}

void State::onExit()
{
  _p->onExit();
}

void State::addTransition(Transition *tr)
{
  _p->addTransition(tr);
}

void State::removeTransition(Transition *tr)
{
  _p->removeTransition(tr);
}

std::list<Transition*>& State::getTransitions()
{
  return _p->_transitions;
}

void State::setDiagram(Diagram* d)
{
  _p->setDiagram(d);
}

Diagram* State::getDiagram()
{
  return _p->_diagram;
}
