/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef TRANSITION_H_
# define TRANSITION_H_

# include <string>

# include <qicore/api.hpp>

namespace qi
{

class TransitionPrivate;
class StateMachine;
class Box;

/// A transition from one State to another
class QICORE_API Transition
{
  public:
    /// Constructor, take the initial State as parameter
    Transition(Box* to);
    ~Transition();

    /// Change the name of the transition
    void setName(std::string name);
    /// Return the name of the transition
    std::string getName() const;

    /// Trigger the transition, if that transition is loaded in a state machine
    void trigger();

    /// Return the initial state
    Box* getFromState() const;
    /// Return the targeted state
    Box* getToState() const;

    TransitionPrivate* _p;
};

}

#endif /* !TRANSITION_H_ */
