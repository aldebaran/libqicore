/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef CONTENT_P_H_
#define CONTENT_P_H_

#include <alserial/alserial.h>
#include <qitype/genericvalue.hpp>

namespace qi
{

  class ContentModelPrivate
  {
    friend class ContentModel;
  public:
    ContentModelPrivate();
    ContentModelPrivate(int type, const std::string &path, const std::string &checksum);
    ContentModelPrivate(boost::shared_ptr<const AL::XmlElement> elt, const std::string &dir);

  private:
    int _type;
    std::string _path;
    std::string _checksum;
    std::string _dir;

  };
}

#endif /* !CONTENT_P_H_ */
