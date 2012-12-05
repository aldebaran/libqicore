/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <limits>
#include <algorithm>

#include <qi/log.hpp>

#include <qicore/transition.hpp>
#include <qicore/statemachine.hpp>

#include "statemachine_private.hpp"
#include "transition_private.hpp"
#include "box_private.hpp"

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

bool StateMachinePrivate::goToState(Box* state)
{
  std::set<Box*>::iterator it = _states.find(state);

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

    state->_p->load();
    if (_currentState)
      _currentState->_p->unload();

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

void StateMachinePrivate::setupTimeOut(unsigned int time)
{
  /* must be in ms */
  qiLogDebug("qiCore.StateMachine") << "Timed transition in : " << time << std::endl;
  setInterval(time);
  playExecuter();
}

void StateMachinePrivate::run()
{
  qiLogDebug("qiCore.StateMachine") << "Starting StateMachine : " << _name;

  { /* Locked Section */
    boost::mutex::scoped_lock lock(_isRunningMutex);

    _isRunning = true;
  } /* End locked Section */

  goToState(_initialState);

  qiLogDebug("qiCore.StateMachine") << "StateMachine Started : " << _name;
}

void StateMachinePrivate::stop()
{
  qiLogDebug("qiCore.StateMachine") << "StateMachine Stopping : " << _name;

  { /* Locked Section */
    boost::mutex::scoped_lock lock(_isRunningMutex);

    _isRunning = false;
  } /* End locked Section */

  { /* Locked Section */
    boost::recursive_mutex::scoped_lock currentStateLock(_currentStateMutex);

    if (_currentState)
    {
      unloadTransitions();
      _currentState->_p->unload();
    }
    _currentState = 0;
  } /* End locked Section */

  _isRunningCondition.notify_all();

  qiLogDebug("qiCore.StateMachine") << "StateMachine Stopped : " << _name;
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
}

bool StateMachine::goToState(Box *state)
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

Box* StateMachine::getCurrentState() const
{
  return _p->_currentState;
}

};
