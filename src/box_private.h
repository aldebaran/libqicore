/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef BOX_PRIVATE_H_
# define BOX_PRIVATE_H_

/* REMOVE ME PLEASE */
# include <newframemanager/newinaoqi.h>

# include <newframemanager/state_machine.h>
# include <newframemanager/timeline.h>

class BoxPrivate: public AL::behavior
{
  friend class Box;

  public:
    BoxPrivate(boost::shared_ptr<AL::ALBroker> broker, std::string name);
    ~BoxPrivate();

  private:
    StateMachine*         _stateMachine;
    Timeline*             _timeline;
};

#endif /* !BOX_PRIVATE_H_ */
