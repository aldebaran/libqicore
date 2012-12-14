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
    _isPaused (false),
    _isRunning (false),
    _isRunningMutex (),
    _initialState (0),
    _currentState (0),
    _timedTransition (0),
    _parent (s),
    _newStateCallback (0)
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
  if (_isPaused)
  {
    qiLogDebug("qiCore.StateMachine") << "StateMachine is paused, transition dropped." << std::endl;
    return false;
  }

  { /* Locked Section */
    boost::recursive_mutex::scoped_lock currentStateLock(_currentStateMutex);

    /*
     * Verify that transition is valid, useful when two transitions
     * are triggered at the same time
     */
    if (_currentState != tr->getFromState())
    {
      qiLogDebug("qiCore.StateMachine") << "Transition expired, transition dropped." << std::endl;
      return false;
    }

    return goToState(tr->getToState());
  } /* End locked Section */
}

bool StateMachinePrivate::goToState(Box* state)
{
  std::set<Box*>::iterator it = _states.find(state);

  /* Is State present in that StateMachine ? */
  if (it == _states.end())
    return false;

  /* Stop the timeOut timer if needed */
  if (_timedTransition != 0)
    pauseExecuter();

  Box* toLoad = state;
  Box* toUnload = 0;
  int timeOut = -1;

  qiLogDebug("qiCore.StateMachine") << "Transition from state: " << (_currentState ? _currentState->getName() : "Null")
                                    << " to state: " << state->getName() << std::endl;

  { /* Locked Section */
    boost::recursive_mutex::scoped_lock currentStateLock(_currentStateMutex);

    toUnload = _currentState;
    unloadTransitions(toUnload);
    timeOut = loadTransitions(toLoad);
    _currentState = state;

  } /* End locked Section */

  if (toLoad)
  {
    toLoad->_p->load();
  }
  if (toUnload)
    toUnload->_p->unload();

  _newStateCallback();

  if (timeOut != -1)
    setupTimeOut(timeOut);

  qiLogDebug("qiCore.StateMachine") << "Transition Done";
  return true;
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

int StateMachinePrivate::goToLabel(int label)
{
  qiLogDebug("qiCore.StateMachine") << "goToLabel: " << label << std::endl;

  for (std::set<Box*>::const_iterator it = _states.begin();
        it != _states.end(); it++)
  {
    if ((*it)->getIntervalBegin() <= label && (*it)->getIntervalEnd() >= label)
    {
      goToState(*it);
      return (*it)->getIntervalBegin();
    }
  }

  return -1;
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

int StateMachinePrivate::loadTransitions(Box* state)
{
  if (!state)
    return -1;

  std::list<Transition*>& trs = state->getTransitions();
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

  if (_isPaused)
  {
    _isPaused = false;
    return;
  }

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

  goToState(0);

  qiLogDebug("qiCore.StateMachine") << "StateMachine Stopped : " << _name;
}

void StateMachinePrivate::pause()
{
  _isPaused = true;
  _timedTransition = 0;
  pauseExecuter();
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

bool StateMachine::executeTransition(Transition *tr)
{
  return _p->executeTransition(tr);
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

};
