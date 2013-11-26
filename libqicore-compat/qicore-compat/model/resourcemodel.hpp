/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef RESSOURCE_H_
#define RESSOURCE_H_

#include <qicore-compat/api.hpp>
#include <string>
#include <alserial/alserial.h>
#include <boost/noncopyable.hpp>

namespace qi
{
  class ResourceModelPrivate;

  class QICORECOMPAT_API ResourceModel : private boost::noncopyable
  {
  public:
    enum LockType {
      LockType_Lock             = 0,
      LockType_StopOnDemand     = 1,
      LockType_CallbackOnDemand = 2,
      LockType_PauseOnDemand    = 3,
      LockType_Error            = -1
    };

    ResourceModel(const std::string &name, ResourceModel::LockType lock_type, int timeout);
    ResourceModel(boost::shared_ptr<const AL::XmlElement> elt);
    virtual ~ResourceModel();

    const std::string& name() const;
    const std::string& lockTypeString() const;
    LockType lockType() const;
    int timeout() const;

    void setName(const std::string& name);
    void setLockType(ResourceModel::LockType lock_type);
    void setTimeout(int timeout);

  private:
    ResourceModelPrivate* _p;
  };
  typedef boost::shared_ptr<ResourceModel> ResourceModelPtr;
}
#endif /* !RESSOURCE_H_ */
