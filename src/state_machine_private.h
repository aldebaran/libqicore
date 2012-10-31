/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef STATE_MACHINE_PRIVATE_H_
# define STATE_MACHINE_PRIVATE_H_

# include <set>

# include <boost/thread/mutex.hpp>

# include "async_executer.h"

class State;
class StateMachine;
class Transition;

class StateMachinePrivate : public asyncExecuter
{
  friend class StateMachine;

  public:
    StateMachinePrivate(StateMachine* s);
    ~StateMachinePrivate();

    bool addState(State* state);
    bool removeState(State* state);

    bool setInitialState(State* initial);
    bool setFinalState(State* final);
    bool removeFinalState(State* final);

    bool isOnFinalState() const;

    void run();
    void stop();

    bool executeTransition(Transition* tr);
    bool goToState(State* state);

    void waitUntilStop();

  protected:
    /* CallBack function for timed transitions */
    virtual bool update();

  private:
    int loadTransitions();
    void unloadTransitions();
    void loadDiagram(State* newState);
    void setupTimeOut(unsigned int time);

    std::string                     _name;
    bool                            _isRunning;
    boost::mutex                    _isRunningMutex;
    boost::condition_variable       _isRunningCondition;
    std::set<State*>                _states;
    std::set<State*>                _finalStates;
    State*                          _initialState;
    State*                          _currentState;
    boost::recursive_mutex          _currentStateMutex;
    Transition*                     _timedTransition;
    StateMachine*                   _parent;
};

#endif /* !STATE_MACHINE_PRIVATE_H_ */
