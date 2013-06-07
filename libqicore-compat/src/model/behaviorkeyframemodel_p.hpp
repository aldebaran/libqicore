/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef BEHAVIORKEYFRAME_P_H_
#define BEHAVIORKEYFRAME_P_H_

#include <alserial/alserial.h>
#include <string>
#include <qicore-compat/model/boxinstancemodel.hpp>
#include <qicore-compat/model/flowdiagrammodel.hpp>

namespace qi {

  class BehaviorKeyFrameModelPrivate
  {
    friend class BehaviorKeyFrameModel;
  public:
    BehaviorKeyFrameModelPrivate();
    BehaviorKeyFrameModelPrivate(const std::string &name,
                                 int index,
                                 const std::string &bitmap,
                                 const std::string &path);
    BehaviorKeyFrameModelPrivate(boost::shared_ptr<const AL::XmlElement> elt, const std::string &dir, boost::shared_ptr<BoxInstanceModel> parent);

  private:
    std::string _name;
    int _index;
    std::string _bitmap;
    std::string _path;
    BoxInstanceModelPtr _parent;
    FlowDiagramModelPtr _diagram;
  };
}

#endif /* !BEHAVIORKEYFRAME_P_H_ */
