/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef STATE_MACHINE_PRIVATE_H_
# define STATE_MACHINE_PRIVATE_H_

# include <set>
# include <queue>

# include <boost/thread/mutex.hpp>

# include <qicore-compat/statemachine.hpp>
# include "asyncexecuter.hpp"
# include "pythoncallback.hpp"

namespace qi
{

class Box;
class StateMachine;
class Transition;

class StateMachinePrivate
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

    int goToLabel(std::string label);
    int goToLabel(int label);
    bool goToState(Box* state);
    bool goToStateName(std::string name);
    bool executeTransition(Transition* tr);

    /* Function will be called when StateMachine entre a new state */
    void registerNewStateCallback(PyObject*);

  private:
    void updateState(Box* state);
    void loadTransitions(Box* state);
    void unloadTransitions(Box* state);
    bool update();

    asyncExecuter*                  _executer;
    std::string                     _name;
    bool                            _isPaused;
    std::set<Box*>                  _states;
    std::set<Box*>                  _finalStates;
    Box*                            _initialState;
    Box*                            _currentState;
    StateMachine*                   _parent;
    PythonCallback                  _newStateCallback;
    unsigned int                    _executerInterval;
    std::queue<Box*>                _nextStateQueue;
    boost::mutex                    _nextStateQueueMutex;
};

};

#endif /* !STATE_MACHINE_PRIVATE_H_ */
