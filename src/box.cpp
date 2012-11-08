/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <alproxies/almemoryproxy.h>
#include <alcore/alerror.h>
#include <qi/log.hpp>

#include <qicore/box.hpp>
#include "box_private.hpp"

namespace qi
{

BoxPrivate::BoxPrivate()
  : _stateMachine (0),
    _timeline (0),
    _name ("Unnamed-Box"),
    _path ("./")
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

Timeline* Box::getTimeline() const
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

StateMachine* Box::getStateMachine() const
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

std::string Box::getName() const
{
  return _p->_name;
}

void Box::setPath(std::string path)
{
  _p->_path = path;
}

std::string Box::getPath() const
{
  return _p->_path;
}

};
