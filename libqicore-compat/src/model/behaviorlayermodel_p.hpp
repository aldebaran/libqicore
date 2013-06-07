/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef BEHAVIORLAYER_P_H_
#define BEHAVIORLAYER_P_H_

#include <alserial/alserial.h>
#include <qicore-compat/model/behaviorkeyframemodel.hpp>

namespace qi {
  class BehaviorLayerModelPrivate
  {
    friend class BehaviorLayerModel;
  public:
    BehaviorLayerModelPrivate();
    BehaviorLayerModelPrivate(boost::shared_ptr<const AL::XmlElement> elt, const std::string &dir, boost::shared_ptr<BoxInstanceModel> parent);
    BehaviorLayerModelPrivate(const std::string &name,
                              bool mute,
                              const std::list<BehaviorKeyFrameModelPtr> &keys);

  private:
    std::string _name;
    bool _mute;
    std::list<BehaviorKeyFrameModelPtr> _behaviorsKeyFrame;
  };
}

#endif /* !BEHAVIORLAYER_P_H_ */
