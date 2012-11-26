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

    BoxPrivate*           _p;
};

};

#endif /* !BOX_H_ */
