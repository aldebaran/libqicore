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

class QICORE_API Box
{
  public:
    Box();
    ~Box();

    void setName(std::string name);
    std::string getName() const;

    void setPath(std::string path);
    std::string getPath() const;

    Timeline* getTimeline() const;
    void setTimeline(Timeline*);
    bool hasTimeline() const;

    StateMachine* getStateMachine() const;
    void setStateMachine(StateMachine*);
    bool hasStateMachine() const;

    void registerOnLoadCallback(PyObject* callable);
    void registerOnUnloadCallback(PyObject* callable);

    /* Allow a box to be a state in StateMachine */
    void addTransition(Transition* tr);
    void removeTransition(Transition* tr);

    std::list<Transition*>& getTransitions() const;

    /* These methods allow to support legacy GoTo */
    void addLabel(std::string label);
    const std::vector<std::string>& getLabels() const;
    void setInterval(unsigned int a, unsigned int b);
    unsigned int getIntervalBegin();
    unsigned int getIntervalEnd();

    BoxPrivate*           _p;
};

};

#endif /* !BOX_H_ */
