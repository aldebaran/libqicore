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
    void pause();
    void stop();

    bool executeTransition(Transition* tr);
    bool goToState(Box* state);
    int goToLabel(std::string label);
    int goToLabel(int label);

  protected:
    /* CallBack function for timed transitions */
    virtual bool update();

  private:
    int loadTransitions(Box* state);
    void unloadTransitions(Box* state);
    void setupTimeOut(unsigned int time);

    std::string                     _name;
    bool                            _isPaused;
    bool                            _isRunning;
    boost::mutex                    _isRunningMutex;
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
