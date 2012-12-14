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

# include "pythoncallback.hpp"

namespace qi
{

class BoxPrivate
{
  friend class Box;

  public:
    BoxPrivate(Box* parent);
    ~BoxPrivate();

    void addTransition(Transition* tr);
    void removeTransition(Transition* tr);

    void load();
    void unload();

    void registerOnLoadCallback(PyObject* callable);
    void registerOnUnloadCallback(PyObject* callable);

  private:
    Box*                  _parent;
    StateMachine*         _stateMachine;
    Timeline*             _timeline;
    std::string           _name;
    std::string           _path;
    PythonCallback        _onLoadCallback;
    PythonCallback        _onUnloadCallback;

    std::list<Transition*>          _transitions;
    std::vector<std::string>        _labels;
    int                    _begin;
    int                    _end;
};

};

#endif /* !BOX_PRIVATE_H_ */
