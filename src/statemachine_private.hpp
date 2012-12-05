/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef STATE_MACHINE_PRIVATE_H_
# define STATE_MACHINE_PRIVATE_H_

# include <set>

# include <boost/thread/mutex.hpp>

# include "asyncexecuter.hpp"

namespace qi
{

class Box;
class StateMachine;
class Transition;

class StateMachinePrivate : public asyncExecuter
{
  friend class StateMachine;

  public:
    StateMachinePrivate(StateMachine* s);
    ~StateMachinePrivate();

    bool addState(Box* state);
    bool removeState(Box* state);

    bool setInitialState(Box* initial);
    bool setFinalState(Box* final);
    bool removeFinalState(Box* final);

    bool isOnFinalState() const;

    void run();
    void stop();

    bool executeTransition(Transition* tr);
    bool goToState(Box* state);

    void waitUntilStop();

  protected:
    /* CallBack function for timed transitions */
    virtual bool update();

  private:
    int loadTransitions();
    void unloadTransitions();
    void setupTimeOut(unsigned int time);

    std::string                     _name;
    bool                            _isRunning;
    boost::mutex                    _isRunningMutex;
    boost::condition_variable       _isRunningCondition;
    std::set<Box*>                  _states;
    std::set<Box*>                  _finalStates;
    Box*                            _initialState;
    Box*                            _currentState;
    boost::recursive_mutex          _currentStateMutex;
    Transition*                     _timedTransition;
    StateMachine*                   _parent;
};

};

#endif /* !STATE_MACHINE_PRIVATE_H_ */
