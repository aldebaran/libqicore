/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef CONTENTS_H_
#define CONTENTS_H_

#include <alserial/alserial.h>
#include <qicore-compat/api.hpp>

namespace qi
{
  class ContentsModelPrivate;
  class ContentModel;

  class QICORECOMPAT_API ContentsModel : private boost::noncopyable
  {
  public:
    ContentsModel(const std::list<boost::shared_ptr<ContentModel> > &contents = std::list<boost::shared_ptr<ContentModel> >());
    ContentsModel(boost::shared_ptr<const AL::XmlElement> elt, const std::string &dir);
    virtual ~ContentsModel();

    const std::list<boost::shared_ptr<ContentModel> >& contents() const;
    void addContent(boost::shared_ptr<ContentModel> content);

    boost::shared_ptr<ContentModel> findContent(int type) const;

  private:
    ContentsModelPrivate* _p;
  };
  typedef boost::shared_ptr<ContentsModel> ContentsModelPtr;
}

#endif /* !CONTENTS_H_ */
