/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef CONTENTS_P_H_
#define CONTENTS_P_H_

#include <qicore-compat/model/contentmodel.hpp>

namespace qi
{
  class ContentsModelPrivate
  {
    friend class ContentsModel;
  public:
    ContentsModelPrivate();
    ContentsModelPrivate(boost::shared_ptr<const AL::XmlElement> elt, const std::string &dir);
    ContentsModelPrivate(std::list<ContentModelPtr> contents);

  private:
    std::list<ContentModelPtr> _contents;
  };
}

#endif /* !CONTENTS_P_H_ */
