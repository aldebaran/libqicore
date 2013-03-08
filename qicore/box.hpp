/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef BOX_H_
# define BOX_H_

# include <Python.h>

# include <alcommon/albroker.h>

# include <qicore/api.hpp>

namespace qi
{

class BoxPrivate;
class Timeline;
class StateMachine;
class Transition;

/// Legacy Box, can be a State in StateMachine
class QICORE_API Box
{
  public:
    Box();
    ~Box();

    /// Change name of the Box
    void setName(std::string name);
    /// Return the name of the Box
    std::string getName() const;

    /// Change the path of the Box
    void setPath(std::string path);
    /// Return the path of the Box
    std::string getPath() const;

    /// Return the timeline associated with the box
    Timeline* getTimeline() const;
    /// Change the timeline associated with the box
    void setTimeline(Timeline*);
    /// True if the box has a Timeline, false otherwise
    bool hasTimeline() const;

    /// Return the StateMachine associated with the box
    StateMachine* getStateMachine() const;
    /// Change the StateMachine associated with the box
    void setStateMachine(StateMachine*);
    /// True if the box has a StateMachine, false otherwise
    bool hasStateMachine() const;

    /* Load and Unload support in Python world */

    /// Register a python callable that will be call on box loading
    void registerOnLoadCallback(PyObject* callable);
    /// Register a python callable that will be call on box unloading
    void registerOnUnloadCallback(PyObject* callable);

    /* Allow a box to be a state in StateMachine */

    /// Add a Transition to this box
    void addTransition(Transition* tr);
    /// Remove Transition to this box
    void removeTransition(Transition* tr);
    /// Return all transitions in a list
    std::list<Transition*>& getTransitions() const;

    /* These methods allow to support legacy GoTo */

    /// Add a label to the box
    void addLabel(std::string label);
    /// Return a vector with all labels
    const std::vector<std::string>& getLabels() const;
    /// Set interval in which the box is supposed to be played
    void setInterval(int a, int b);
    /// Return the lower bound of the interval
    int getIntervalBegin();
    /// Return the upper bound of the interval
    int getIntervalEnd();

    BoxPrivate*           _p;
};

}

#endif /* !BOX_H_ */
