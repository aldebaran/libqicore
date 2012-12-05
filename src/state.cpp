/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <qi/log.hpp>

#include "box_private.hpp"
#include "state_private.hpp"
#include "transition_private.hpp"
#include "statemachine_private.hpp"

namespace qi
{

StatePrivate::StatePrivate(State* parent)
  : _name ("Unnamed-State"),
    _parent (parent)
{
}

StatePrivate::~StatePrivate()
{
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

void StatePrivate::addBox(Box *b)
{
  _boxes.insert(b);
}

void StatePrivate::removeBox(Box *b)
{
  _boxes.erase(b);
}

void StatePrivate::loadAllBoxes()
{
  for (std::set<Box*>::iterator it = _boxes.begin();
        it != _boxes.end(); it++)
  {
    loadBox(*it);
  }
}

void StatePrivate::unloadAllBoxes()
{
  for (std::set<Box*>::iterator it = _boxes.begin();
        it != _boxes.end(); it++)
  {
    unloadBox(*it);
  }
}

void StatePrivate::loadBox(Box* b)
{
  qiLogDebug("qiCore.Diagram") << "Load the box named: " << b->getName() << std::endl;
  b->_p->load();
}

void StatePrivate::unloadBox(Box *b)
{
  qiLogDebug("qiCore.Diagram") << "Unload the box named: " << b->getName() << std::endl;
  b->_p->unload();
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

void State::addTransition(Transition *tr)
{
  _p->addTransition(tr);
}

void State::removeTransition(Transition *tr)
{
  _p->removeTransition(tr);
}

std::list<Transition*>& State::getTransitions() const
{
  return _p->_transitions;
}

void State::setName(std::string name)
{
  _p->_name = name;
}

std::string State::getName() const
{
  return _p->_name;
}

void State::addBox(Box *b)
{
  _p->addBox(b);
}

void State::removeBox(Box *b)
{
  _p->removeBox(b);
}

};
