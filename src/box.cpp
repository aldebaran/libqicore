/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <alproxies/almemoryproxy.h>
#include <alcore/alerror.h>
#include <qi/log.hpp>

#include <qicore/box.hpp>
#include "box_private.hpp"

BoxPrivate::BoxPrivate()
  : _stateMachine (0),
    _timeline (0)
{
}

BoxPrivate::~BoxPrivate()
{
}

void BoxPrivate::load()
{
  /* Rewrite this method for naoqi2 */
  boost::shared_ptr<AL::ALMemoryProxy> mem = _broker->getMemoryProxy();

  try
  {
    mem->raiseMicroEvent(_name + "____Internal__OnLoad", AL::ALValue());
  }
  catch (AL::ALError& e)
  {
    std::stringstream ss;
    ss << "Error During STM access : Error #=" << e.toString();
    qiLogError("qiCore") <<  ss.str() << std::endl;
  }
}

void BoxPrivate::unload()
{
  /* Rewrite this method for naoqi2 */
  boost::shared_ptr<AL::ALMemoryProxy> mem = _broker->getMemoryProxy();

  try
  {
    mem->raiseMicroEvent(_name + "____Internal__OnUnload", AL::ALValue());
  }
  catch (AL::ALError& e)
  {
    std::stringstream ss;
    ss << "Error During STM access : Error #=" << e.toString();
    qiLogError("qiCore") <<  ss.str() << std::endl;
  }

}

/* Public Class */
Box::Box()
  : _p (new BoxPrivate())
{
}

Box::~Box()
{
  delete _p;
}

Timeline* Box::getTimeline()
{
  return _p->_timeline;
}

void Box::setTimeline(Timeline* t)
{
  _p->_timeline = t;
}

bool Box::hasTimeline() const
{
  return _p->_timeline;
}

StateMachine* Box::getStateMachine()
{
  return _p->_stateMachine;
}

void Box::setStateMachine(StateMachine* s)
{
  _p->_stateMachine = s;
}

bool Box::hasStateMachine() const
{
  return _p->_stateMachine;
}

void Box::setBroker(boost::shared_ptr<AL::ALBroker> broker)
{
  _p->_broker = broker;
}

void Box::setName(std::string name)
{
  _p->_name = name;
}

std::string Box::getName()
{
  return _p->_name;
}

void Box::load()
{
  _p->load();
}

void Box::unload()
{
  _p->unload();
}
