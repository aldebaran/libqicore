/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <limits>
#include <algorithm>

#include <qi/log.hpp>

#include <qicore/state.hpp>
#include <qicore/transition.hpp>
#include <qicore/diagram.hpp>
#include <qicore/statemachine.hpp>

#include "statemachine_private.hpp"
#include "transition_private.hpp"
#include "diagram_private.hpp"

namespace qi
{

StateMachinePrivate::StateMachinePrivate(StateMachine *s)
  : asyncExecuter(42),
    _name ("Unnamed-StateMachine"),
    _isRunning (false),
    _isRunningMutex (),
    _isRunningCondition(),
    _initialState (0),
    _currentState (0),
    _timedTransition (0),
    _parent (s)
{
}

StateMachinePrivate::~StateMachinePrivate()
{
}

bool StateMachinePrivate::addState(State* state)
{
  _states.insert(state);
  return true;
}

bool StateMachinePrivate::removeState(State* state)
{
  std::set<State*>::iterator it = _states.find(state);

  if (it == _states.end())
    return false;

  if ((*it) == _initialState)
    _initialState = 0;

  std::set<State*>::iterator itSet = _finalStates.find(state);
  if (itSet != _finalStates.end())
    _finalStates.erase(itSet);

  _states.erase(it);

  return true;
}

bool StateMachinePrivate::setInitialState(State* initial)
{
  std::set<State*>::iterator it = _states.find(initial);

  if (it == _states.end())
    return false;

  _initialState = initial;
  return true;
}

bool StateMachinePrivate::setFinalState(State* final)
{
  std::set<State*>::iterator it = _states.find(final);

  if (it == _states.end())
    return false;

  _finalStates.insert(final);
  return true;
}

bool StateMachinePrivate::removeFinalState(State* final)
{
  std::set<State*>::iterator itSet = _finalStates.find(final);

  if (itSet == _finalStates.end())
    return false;

  _finalStates.erase(itSet);
  return true;
}

bool StateMachinePrivate::isOnFinalState() const
{
  std::set<State*>::iterator itSet = _finalStates.find(_currentState);

  return (itSet != _finalStates.end());
}

bool StateMachinePrivate::executeTransition(Transition* tr)
{
  { /* Locked Section */
    boost::recursive_mutex::scoped_lock currentStateLock(_currentStateMutex);

    /*
     * Verify that transition is valid, useful when two transitions
     * are triggered at the same time
     */
    if (_currentState != tr->getFromState())
      return false;

    return goToState(tr->getToState());
  } /* End locked Section */
}

bool StateMachinePrivate::goToState(State* state)
{
  std::set<State*>::iterator it = _states.find(state);

  /* Is State present in that StateMachine ? */
  if (it == _states.end())
    return false;

  { /* Locked Section */
    boost::recursive_mutex::scoped_lock currentStateLock(_currentStateMutex);
    qiLogDebug("qiCore.StateMachine") << "Transition from state: " << (_currentState ? _currentState->getName() : "Null")
                                      << " to state: " << state->getName() << std::endl;

    /* Stop the timeOut timer if needed */
    if (_timedTransition != 0)
      stopExecuter();

    if (_currentState)
      unloadTransitions();

    /* ReLoad Diagram */
    loadDiagram(state);

    _currentState = state;
    /* Load Transitions */
    int timeOut = loadTransitions();
    if (timeOut != -1)
      setupTimeOut(timeOut);

  } /* End locked Section */

  return true;
}

bool StateMachinePrivate::update()
{
  pauseExecuter();

  Transition* tr = _timedTransition;
  _timedTransition = 0;
  if (tr)
  {
    qiLogDebug("qiCore.StateMachine") << "Triggering the timed transition" << std::endl;
    tr->trigger();
  }

  return true;
}

int StateMachinePrivate::loadTransitions()
{
  std::list<Transition*>& trs = _currentState->getTransitions();
  int timeOut = -1;

  for (std::list<Transition*>::iterator it = trs.begin();
        it != trs.end(); it++)
  {
    /* Set State machine for Event Callback */
    (*it)->_p->load(_parent);

    if ((*it)->hasTimeOut())
    {
      if (timeOut == -1)
      {
        timeOut = (*it)->getTimeOut();
        _timedTransition = *it;
      }
      else if ((*it)->getTimeOut() < timeOut)
      {
        timeOut = (*it)->getTimeOut();
        _timedTransition = *it;
      }
    }
  }

  return timeOut;
}

void StateMachinePrivate::unloadTransitions()
{
  std::list<Transition*>& trs = _currentState->getTransitions();

  for (std::list<Transition*>::iterator it = trs.begin();
        it != trs.end(); it++)
  {
    (*it)->_p->unload();
  }
}

void StateMachinePrivate::loadDiagram(State* newState)
{
  if (_currentState)
  {
    if (newState->getDiagram())
      newState->getDiagram()->_p->loadFromDiagram(_currentState->getDiagram());
  }
  else
  {
    if (newState->getDiagram())
      newState->getDiagram()->_p->loadAllBoxes();
  }
}

void StateMachinePrivate::setupTimeOut(unsigned int time)
{
  /* must be in ms */
  qiLogDebug("qiCore.StateMachine") << "Timed transition in : " << time << std::endl;
  setInterval(time);
  playExecuter();
}

void StateMachinePrivate::run()
{
  { /* Locked Section */
    boost::mutex::scoped_lock lock(_isRunningMutex);

    _isRunning = true;
  } /* End locked Section */

  goToState(_initialState);
}

void StateMachinePrivate::stop()
{
  { /* Locked Section */
    boost::mutex::scoped_lock lock(_isRunningMutex);

    _isRunning = false;
  } /* End locked Section */

  { /* Locked Section */
    boost::recursive_mutex::scoped_lock currentStateLock(_currentStateMutex);

    if (_currentState)
    {
      unloadTransitions();
      if (_currentState->getDiagram())
        _currentState->getDiagram()->_p->unloadAllBoxes();
    }
    _currentState = 0;
  } /* End locked Section */

  _isRunningCondition.notify_all();
}

void StateMachinePrivate::waitUntilStop()
{
  boost::mutex::scoped_lock lock(_isRunningMutex);

  if (_isRunning)
    _isRunningCondition.wait(lock);
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

bool StateMachine::addState(State* state)
{
  return _p->addState(state);
}

bool StateMachine::removeState(State* state)
{
  return _p->removeState(state);
}

bool StateMachine::setInitialState(State* initial)
{
  return _p->setInitialState(initial);
}

bool StateMachine::setFinalState(State* final)
{
  return _p->setFinalState(final);
}

bool StateMachine::removeFinalState(State* final)
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
}

bool StateMachine::goToState(State *state)
{
  return _p->goToState(state);
}

bool StateMachine::executeTransition(Transition *tr)
{
  return _p->executeTransition(tr);
}

void StateMachine::waitUntilStop()
{
  _p->waitUntilStop();
}

void StateMachine::setName(std::string name)
{
  _p->_name = name;
}

std::string StateMachine::getName() const
{
  return _p->_name;
}

State* StateMachine::getCurrentState() const
{
  return _p->_currentState;
}

};
