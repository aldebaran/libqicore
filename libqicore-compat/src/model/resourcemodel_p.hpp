/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef RESSOURCE_P_H
#define RESSOURCE_P_H

#include <string>
#include <alserial/alserial.h>
#include <qicore-compat/model/resourcemodel.hpp>

namespace qi
{
  class ResourceModelPrivate
  {
    friend class ResourceModel;
  public:
    ResourceModelPrivate(const std::string &name, ResourceModel::LockType lock_type, int timeout);
    ResourceModelPrivate(boost::shared_ptr<const AL::XmlElement> elt);

  private:
    std::string _name;
    int         _lockType;
    int         _timeout;
    std::string _type;
  };
}

#endif /* !RESSOURCE_P_H */
