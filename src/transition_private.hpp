/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef TRANSITION_PRIVATE_H_
# define TRANSITION_PRIVATE_H_

class State;
class StateMachine;
class Transition;

class TransitionPrivate
{
  friend class Transition;

  public:
    TransitionPrivate(Transition* tr, State* toState);
    virtual ~TransitionPrivate();

    void triggerTransition();
    void setMachine(StateMachine* s);
    void setFromState(State* s);

    State* getFromState();
    State* getToState();

  private:
    std::string   _name;
    bool          _hasTimeOut;
    int           _timeOut;
    StateMachine* _machine;
    State*        _fromState;
    State*        _toState;
    Transition*   _parent;
};

#endif /* !TRANSITION_PRIVATE_H_ */
