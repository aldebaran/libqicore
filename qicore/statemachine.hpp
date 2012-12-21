/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef STATE_MACHINE_H_
# define STATE_MACHINE_H_

# include <string>
# include <qicore/api.hpp>
# include <Python.h>

namespace qi
{

class StateMachinePrivate;
class Transition;
class Box;

/// StateMachine class
class QICORE_API StateMachine
{
  public:
    StateMachine();
    ~StateMachine();

    /// Change the name of the StateMachine
    void setName(std::string name);
    /// Return the name of the StateMachine
    std::string getName() const;

    /// Add a state in the StateMachine
    bool addState(Box* state);
    /// Remove a state in the StateMachine
    bool removeState(Box* state);

    /// Set a state to be the first state
    bool setInitialState(Box* initial);
    /// Set a state to be one of the final states
    bool setFinalState(Box* final);
    /// Remove the status of final state to a state
    bool removeFinalState(Box* final);

    /// True if the stateMachine is currently on a final state
    bool isOnFinalState() const;

    /// Run the StateMachine, go to the initial state
    void run();
    /// Pause the StateMachine
    void pause();
    /// Stop the StateMachine, set the current state to zero
    void stop();

    /// Go to a state
    bool goToState(Box* state);
    /// Go to the named state
    bool goToStateName(std::string name);
    /// Go to the state designed by the label
    int goToLabel(std::string label);
    /// Go to the state designed by the int label
    int goToLabel(int label);
    /// Execute a transition from a state to another
    bool executeTransition(Transition* tr);

    /// Return the current state
    Box* getCurrentState() const;

    /// Register a python callable that will be called when entering a new state
    void registerNewStateCallback(PyObject *);

  private:
    StateMachinePrivate        *_p;
};

};

#endif /* !STATE_MACHINE_H_ */
