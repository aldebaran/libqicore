/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef BEHAVIORKEYFRAME_H_
#define BEHAVIORKEYFRAME_H_

#include <qicore-compat/api.hpp>
#include <alserial/alserial.h>
#include <boost/noncopyable.hpp>

namespace qi {

  class BehaviorKeyFrameModelPrivate;
  class FlowDiagramModel;

  class QICORECOMPAT_API BehaviorKeyFrameModel : private boost::noncopyable
  {
  public:
    BehaviorKeyFrameModel(const std::string &name, int index, const std::string &bitmap, const std::string &path);
    BehaviorKeyFrameModel(boost::shared_ptr<const AL::XmlElement> elt, const std::string &dir, boost::shared_ptr<BoxInstanceModel> parent);
    virtual ~BehaviorKeyFrameModel();

    const std::string& name() const;
    int index() const;
    const std::string& bitmap() const;
    std::string path() const;

    boost::shared_ptr<FlowDiagramModel> diagram();

    void setName(const std::string& name);
    void setIndex(int index);
    void setBitmap(const std::string& bitmap);
    void setPath(const std::string& path);

  private:
    BehaviorKeyFrameModelPrivate *_p;
  };
  typedef boost::shared_ptr<BehaviorKeyFrameModel> BehaviorKeyFrameModelPtr;
}

#endif /* !BEHAVIORKEYFRAME_H_ */
