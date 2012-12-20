/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef TRANSITION_PRIVATE_H_
# define TRANSITION_PRIVATE_H_

namespace qi
{

class Box;
class StateMachine;
class Transition;

class TransitionPrivate
{
  friend class Transition;

  public:
    TransitionPrivate(Transition* tr, Box* toState);
    virtual ~TransitionPrivate();

    void triggerTransition();
    void setFromState(Box* s);

    Box* getFromState();
    Box* getToState();

    void load(StateMachine *s);
    void unload();

  private:
    void setMachine(StateMachine* s);

    std::string   _name;
    bool          _hasTimeOut;
    int           _timeOut;
    StateMachine* _machine;
    Box*          _fromState;
    Box*          _toState;
    Transition*   _parent;
};

};

#endif /* !TRANSITION_PRIVATE_H_ */
