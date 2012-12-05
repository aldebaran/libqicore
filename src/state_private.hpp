/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef STATE_PRIVATE_H_
# define STATE_PRIVATE_H_

# include <list>
# include <set>

# include <qicore/state.hpp>
# include <qicore/transition.hpp>

namespace qi
{

class Box;

class StatePrivate
{
  friend class TransitionPrivate;
  friend class State;

  public:
    StatePrivate(State* s);
    ~StatePrivate();

    void addTransition(Transition* tr);
    void removeTransition(Transition* tr);

    void addBox(Box* b);
    void removeBox(Box* b);

    void loadAllBoxes();
    void unloadAllBoxes();

    void loadFromState(State* st);

  private:
    void loadBox(Box* b);
    void unloadBox(Box* b);

    std::string                     _name;
    State*                          _parent;
    std::list<Transition*>          _transitions;
    std::set<Box*> _boxes;
};

};

#endif /* !STATE_PRIVATE_H_ */
