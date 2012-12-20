/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <limits>
#include <algorithm>

#include <qi/log.hpp>

#include <qicore/transition.hpp>
#include <qicore/statemachine.hpp>

#include "statemachine_p.hpp"
#include "transition_p.hpp"
#include "box_p.hpp"

namespace qi
{

StateMachinePrivate::StateMachinePrivate(StateMachine *s)
  : _executer(new asyncExecuter(0)),
    _name ("Unnamed-StateMachine"),
    _isPaused (false),
    _initialState (0),
    _currentState (0),
    _parent (s),
    _newStateCallback (0),
    _executerInterval(1),
    _nextStateQueue(),
    _nextStateQueueMutex()
{
}

StateMachinePrivate::~StateMachinePrivate()
{
  delete _executer;
}

bool StateMachinePrivate::addState(Box* state)
{
  _states.insert(state);
  return true;
}

bool StateMachinePrivate::removeState(Box* state)
{
  std::set<Box*>::iterator it = _states.find(state);

  if (it == _states.end())
    return false;

  if ((*it) == _initialState)
    _initialState = 0;

  std::set<Box*>::iterator itSet = _finalStates.find(state);
  if (itSet != _finalStates.end())
    _finalStates.erase(itSet);

  _states.erase(it);

  return true;
}

bool StateMachinePrivate::setInitialState(Box* initial)
{
  std::set<Box*>::iterator it = _states.find(initial);

  if (it == _states.end())
    return false;

  _initialState = initial;
  return true;
}

bool StateMachinePrivate::setFinalState(Box* final)
{
  std::set<Box*>::iterator it = _states.find(final);

  if (it == _states.end())
    return false;

  _finalStates.insert(final);
  return true;
}

bool StateMachinePrivate::removeFinalState(Box* final)
{
  std::set<Box*>::iterator itSet = _finalStates.find(final);

  if (itSet == _finalStates.end())
    return false;

  _finalStates.erase(itSet);
  return true;
}

bool StateMachinePrivate::isOnFinalState() const
{
  std::set<Box*>::iterator itSet = _finalStates.find(_currentState);

  return (itSet != _finalStates.end());
}

bool StateMachinePrivate::goToState(Box *state)
{
  if (state)
  {
    std::set<Box*>::iterator it = _states.find(state);

    /* Is State present in that StateMachine ? */
    if (it == _states.end())
      return false;
  }

  {
    boost::mutex::scoped_lock lock(_nextStateQueueMutex);

    _nextStateQueue.push(state);
  }

  return true;
}

void StateMachinePrivate::updateState(Box* state)
{
  /* Stop the timeOut timer if needed */
  Box* toLoad = state;
  Box* toUnload = 0;

  qiLogDebug("qiCore.StateMachine") << "Transition from state: " << (_currentState ? _currentState->getName() : "Null")
                                      << " to state: " <<  (state ? state->getName() : "Null") << std::endl;
  toUnload = _currentState;
  unloadTransitions(toUnload);
  loadTransitions(toLoad);
  _currentState = state;

  if (toLoad)
    toLoad->_p->load();
  if (toUnload)
    toUnload->_p->unload();

  _newStateCallback();

  qiLogDebug("qiCore.StateMachine") << "Transition Done";
}

int StateMachinePrivate::goToLabel(std::string label)
{
  qiLogDebug("qiCore.StateMachine") << "goToLabel: " << label << std::endl;

  for (std::set<Box*>::const_iterator it = _states.begin();
        it != _states.end(); it++)
  {
    const std::vector<std::string> labels = (*it)->getLabels();

    for (std::vector<std::string>::const_iterator it2 = labels.begin();
          it2 != labels.end(); it2++)
    {
      if ((*it2) == label)
      {
        if (!goToState(*it))
          return -1;
        return (*it)->getIntervalBegin();
      }
    }
  }

  return -1;
}

bool StateMachinePrivate::goToStateName(std::string name)
{
  qiLogDebug("qiCore.StateMachine") << "goToStateName: " << name << std::endl;

  for (std::set<Box*>::const_iterator it = _states.begin();
        it != _states.end(); it++)
  {
    if ((*it)->getName() == name)
    {
      if (!goToState(*it))
        return -1;
      return (*it)->getIntervalBegin();
    }
  }

  return -1;
}

int StateMachinePrivate::goToLabel(int label)
{
  qiLogDebug("qiCore.StateMachine") << "goToLabel: " << label << std::endl;

  if (label <= 0)
    return -1;

  Box* lastBox = 0;

  for (std::set<Box*>::const_iterator it = _states.begin();
        it != _states.end(); it++)
  {
    if (!lastBox)
      lastBox = *it;
    else
      if ((*it)->getIntervalEnd() > lastBox->getIntervalEnd())
        lastBox = *it;

    if ((*it)->getIntervalBegin() <= label && (*it)->getIntervalEnd() >= label)
    {
      goToState(*it);
      return (*it)->getIntervalBegin();
    }
  }

  /* Label is positif and not in any box interval so it jumps beyond the last state */
  if (lastBox)
  {
    goToState(lastBox);
    return lastBox->getIntervalBegin();
  }

  return -1;
}

bool StateMachinePrivate::executeTransition(Transition *tr)
{
  if (!tr)
    return false;

  Box* toState = tr->getToState();
  return goToState(toState);
}

bool StateMachinePrivate::update()
{
  Box* next = 0;

  /* Take the next state in locked queue */
  {
    boost::mutex::scoped_lock lock(_nextStateQueueMutex);
    if (!_nextStateQueue.empty())
    {
      next = _nextStateQueue.front();
      _nextStateQueue.pop();
    }
  }
  /* If state is not null, go to this state */
  if (next)
    updateState(next);

  return true;
}

void StateMachinePrivate::loadTransitions(Box* state)
{
  if (!state)
    return;

  std::list<Transition*>& trs = state->getTransitions();

  for (std::list<Transition*>::iterator it = trs.begin();
        it != trs.end(); it++)
  {
    /* Set State machine for Event Callback */
    (*it)->_p->load(_parent);
  }
}

void StateMachinePrivate::unloadTransitions(Box* state)
{
  if (!state)
    return;

  std::list<Transition*>& trs = state->getTransitions();

  for (std::list<Transition*>::iterator it = trs.begin();
        it != trs.end(); it++)
  {
    (*it)->_p->unload();
  }
}

void StateMachinePrivate::run()
{
  qiLogDebug("qiCore.StateMachine") << "Starting StateMachine : " << _name;

  if (_isPaused)
  {
    _isPaused = false;
    return;
  }

  if (!_executer->isPlaying())
  {
    /* It is safe to call updateState only when executer is not running */
    updateState(_initialState);
    _executer->setInterval(_executerInterval);
    _executer->playExecuter(boost::bind(&StateMachinePrivate::update, this));
  }

  qiLogDebug("qiCore.StateMachine") << "StateMachine Started : " << _name;
}

void StateMachinePrivate::stop()
{
  qiLogDebug("qiCore.StateMachine") << "StateMachine Stopping : " << _name;

  /* Clear the queue here ? */
  goToState(0);

  qiLogDebug("qiCore.StateMachine") << "StateMachine Stopped : " << _name;
}

void StateMachinePrivate::pause()
{
  /* Just do nothing for the moment */
}

void StateMachinePrivate::registerNewStateCallback(PyObject* p)
{
  _newStateCallback.assignCallback(p);
}

/* -- Public -- */

StateMachine::StateMachine()
  : _p (new StateMachinePrivate(this))
{
}

StateMachine::~StateMachine()
{
  delete _p;
}

bool StateMachine::addState(Box* state)
{
  return _p->addState(state);
}

bool StateMachine::removeState(Box* state)
{
  return _p->removeState(state);
}

bool StateMachine::setInitialState(Box* initial)
{
  return _p->setInitialState(initial);
}

bool StateMachine::setFinalState(Box* final)
{
  return _p->setFinalState(final);
}

bool StateMachine::removeFinalState(Box* final)
{
  return _p->removeFinalState(final);
}

bool StateMachine::isOnFinalState() const
{
  return _p->isOnFinalState();
}

void StateMachine::run()
{
  _p->run();
}

void StateMachine::stop()
{
  _p->stop();
}

void StateMachine::pause()
{
  _p->pause();
}

bool StateMachine::goToState(Box *state)
{
  return _p->goToState(state);
}

bool StateMachine::goToStateName(std::string name)
{
  return _p->goToStateName(name);
}

void StateMachine::setName(std::string name)
{
  _p->_name = name;
}

std::string StateMachine::getName() const
{
  return _p->_name;
}

Box* StateMachine::getCurrentState() const
{
  return _p->_currentState;
}

int StateMachine::goToLabel(std::string label)
{
  return _p->goToLabel(label);
}

int StateMachine::goToLabel(int label)
{
  return _p->goToLabel(label);
}

void StateMachine::registerNewStateCallback(PyObject* p)
{
  _p->registerNewStateCallback(p);
}

bool StateMachine::executeTransition(Transition *tr)
{
  return _p->executeTransition(tr);
}

};
