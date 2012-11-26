/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef BOX_PRIVATE_H_
# define BOX_PRIVATE_H_

# include <qicore/box.hpp>
# include <qicore/statemachine.hpp>
# include <qicore/timeline.hpp>

namespace qi
{

class BoxPrivate
{
  friend class Box;

  public:
    BoxPrivate();
    ~BoxPrivate();

    void load();
    void unload();

    void registerOnLoadCallback(PyObject* callable);
    void registerOnUnloadCallback(PyObject* callable);

  private:
    void invokeCallback(PyObject* callback);

    StateMachine*         _stateMachine;
    Timeline*             _timeline;
    std::string           _name;
    std::string           _path;
    PyObject*             _onLoadCallback;
    PyObject*             _onUnloadCallback;
};

};

#endif /* !BOX_PRIVATE_H_ */
