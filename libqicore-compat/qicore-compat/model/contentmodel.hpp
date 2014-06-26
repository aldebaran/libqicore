/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef CONTENT_H_
#define CONTENT_H_

#include <alserial/alserial.h>
#include <qicore-compat/api.hpp>
#include <qi/anyvalue.hpp>

namespace qi
{
  class ContentModelPrivate;
  class AnimationModel;
  class BehaviorSequenceModel;
  class FlowDiagramModel;
  class BoxInstanceModel;

  class QICORECOMPAT_API ContentModel : private boost::noncopyable
  {
  public:
    enum  ContentType
    {
      ContentType_PythonScript     = 0,
      ContentType_QiChartScript    = 1,
      ContentType_FlowDiagram      = 2,
      ContentType_BehaviorSequence = 3,
      ContentType_Animation        = 4
    };

    ContentModel(ContentType type, const std::string &path, const std::string &checksum);
    ContentModel(boost::shared_ptr<const AL::XmlElement> elt, const std::string &dir);
    virtual ~ContentModel();

    int type() const;
    const std::string& path() const;
    const std::string& checksum() const;

    AnyReference content(boost::shared_ptr<BoxInstanceModel> parent);

    void setType(ContentType type);
    void setPath(const std::string& path);
    void setChecksum(const std::string& checksum);

  private:
    ContentModelPrivate* _p;
  };
  typedef boost::shared_ptr<ContentModel> ContentModelPtr;
}

#endif /* !CONTENT_H_ */
