/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef STATE_MACHINE_H_
# define STATE_MACHINE_H_

# include <string>

# include <qicore/api.hpp>

namespace qi
{

class StateMachinePrivate;
class Transition;
class State;

class QICORE_API StateMachine
{
  public:
    StateMachine();
    ~StateMachine();

    void setName(std::string name);
    std::string getName();

    bool addState(State* state);
    bool removeState(State* state);

    bool setInitialState(State* initial);
    bool setFinalState(State* final);
    bool removeFinalState(State* final);

    bool isOnFinalState() const;

    void run();
    void stop();

    bool goToState(State* state);
    bool executeTransition(Transition* tr);
    State* getCurrentState();

    void waitUntilStop();

  private:
    StateMachinePrivate        *_p;
};

};

#endif /* !STATE_MACHINE_H_ */
