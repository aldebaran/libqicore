/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <string>
#include <boost/shared_ptr.hpp>

#include <qicore-compat/api.hpp>

namespace qi {
  class AnimationModelPrivate;
  class ActuatorListModel;

  class QICORECOMPAT_API AnimationModel {
  public:
    AnimationModel(const std::string &path,
                   int fps = 25,
                   int startFrame = 0,
                   int endFrame = -1,
                   int size = 0,
                   const std::string &formatVersion = "4",
                   boost::shared_ptr<ActuatorListModel> actuatorList = boost::shared_ptr<ActuatorListModel>());
    virtual ~AnimationModel();

    std::string path() const;
    int fps() const;
    int startFrame() const;
    int endFrame() const;
    int size() const;
    const std::string& formatVersion() const;
    boost::shared_ptr<ActuatorListModel> actuatorList() const;

    void setPath(const std::string &path);
    void setFPS(int fps);
    void setStartFrame(int start_frame);
    void setEndFrame(int end_frame);
    void setSize(int size);
    void setFormatVersion(const std::string &format_version);
    void setActuatorList(boost::shared_ptr<ActuatorListModel> actuator_list);

    bool loadFromFile();

  private:
    QI_DISALLOW_COPY_AND_ASSIGN(AnimationModel);
    AnimationModelPrivate* _p;
  };
  typedef boost::shared_ptr<AnimationModel> AnimationModelPtr;
}

#endif /* !ANIMATION_H_ */
