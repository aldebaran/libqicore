/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef TRANSITION_H_
# define TRANSITION_H_

# include <qicore/api.h>

class TransitionPrivate;
class StateMachine;
class State;

class QICORE_API Transition
{
  public:
    friend class StatePrivate;

    Transition(State* to);
    ~Transition();

    void load(StateMachine* s);
    void unload();

    void trigger();

    State* getFromState();
    State* getToState();

    bool hasTimeOut();
    int getTimeOut();
    void setTimeOut(int n);

  private:
    TransitionPrivate* _p;
};

#endif /* !TRANSITION_H_ */
