/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef BEHAVIORLAYER_H_
#define BEHAVIORLAYER_H_

#include <qicore-compat/api.hpp>
#include <alserial/alserial.h>

namespace qi {
  class BehaviorLayerModelPrivate;
  class BehaviorKeyFrameModel;

  class QICORECOMPAT_API BehaviorLayerModel {
  public:
    BehaviorLayerModel(const std::string &name,
                       bool mute,
                       const std::list<boost::shared_ptr<BehaviorKeyFrameModel> > &keys = std::list<boost::shared_ptr<BehaviorKeyFrameModel> >());
    BehaviorLayerModel(boost::shared_ptr<const AL::XmlElement> elt, const std::string &dir);
    virtual ~BehaviorLayerModel();

    const std::string &name() const;
    bool mute() const;
    const std::list<boost::shared_ptr<BehaviorKeyFrameModel> >& behaviorsKeyFrame() const;

    void setName(const std::string &name);
    void setMute(bool mute);
    void addBehaviorKeyFrame(boost::shared_ptr<BehaviorKeyFrameModel> behaviorKeyFrame);

  private:
    QI_DISALLOW_COPY_AND_ASSIGN(BehaviorLayerModel);
    BehaviorLayerModelPrivate* _p;
  };
  typedef boost::shared_ptr<BehaviorLayerModel> BehaviorLayerModelPtr;
}
#endif /* !BEHAVIORLAYER_H_ */
