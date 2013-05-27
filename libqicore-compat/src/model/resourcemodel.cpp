/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <qicore-compat/model/resourcemodel.hpp>
#include "resourcemodel_p.hpp"

#include <qi/log.hpp>
qiLogCategory("QiCore-Compat.ResourceModel");

namespace qi
{
  //----------------------------------------Private Class------------------------------------//
  ResourceModelPrivate::ResourceModelPrivate(const std::string &name, ResourceModel::LockType lock_type, int timeout) :
    _name(name),
    _lockType(lock_type),
    _timeout(timeout)
  {
  }

  ResourceModelPrivate::ResourceModelPrivate(boost::shared_ptr<const AL::XmlElement> elt)
  {
    std::string lock_type;
    elt->getAttribute("name", _name);
    elt->getAttribute("lock_type", lock_type);
    elt->getAttribute("timeout", _timeout);

    if      (lock_type == std::string("Lock"))
      _lockType = ResourceModel::LockType_Lock;
    else if (lock_type == std::string("Stop on demand"))
      _lockType = ResourceModel::LockType_StopOnDemand;
    else if (lock_type == std::string("Callback on demand"))
      _lockType = ResourceModel::LockType_CallbackOnDemand;
    else if (lock_type == std::string("Pause on demand"))
      _lockType = ResourceModel::LockType_PauseOnDemand;
    else
      _lockType = ResourceModel::LockType_Error;

    qiLogDebug() << "Resource : name = "
                 << _name
                 << ", lock_type = "
                 << _lockType << ", " << lock_type
                 << ", timeout = "
                 << _timeout
                 << std::endl;


  }
  //----------------------------------------Public Class------------------------------------//
  ResourceModel::ResourceModel(const std::string &name, ResourceModel::LockType lock_type, int timeout) :
    _p(new ResourceModelPrivate(name, lock_type, timeout))
  {
  }

  ResourceModel::ResourceModel(boost::shared_ptr<const AL::XmlElement> elt) :
    _p(new ResourceModelPrivate(elt))
  {
  }

  ResourceModel::~ResourceModel()
  {
    delete _p;
  }

  const std::string& ResourceModel::lockTypeString() const
  {
    switch(_p->_lockType)
    {
    case LockType_Lock:
      _p->_type = "Lock";
      break;

    case LockType_StopOnDemand:
      _p->_type = "Stop on demand";
      break;

    case LockType_CallbackOnDemand:
      _p->_type = "Callback on demand";
      break;

    case LockType_PauseOnDemand:
      _p->_type = "Pause on demand";
      break;

    default:
      _p->_type = "";
      break;
    }

    return _p->_type;
  }

  ResourceModel::LockType ResourceModel::lockType() const
  {
    return static_cast<LockType>(_p->_lockType);
  }

  const std::string& ResourceModel::name() const
  {
    return _p->_name;
  }

  int ResourceModel::timeout() const
  {
    return _p->_timeout;
  }

  void ResourceModel::setLockType(LockType lock_type)
  {
    _p->_lockType = lock_type;
  }

  void ResourceModel::setName(const std::string& name)
  {
    _p->_name = name;
  }

  void ResourceModel::setTimeout(int timeout)
  {
    _p->_timeout = timeout;
  }

}
