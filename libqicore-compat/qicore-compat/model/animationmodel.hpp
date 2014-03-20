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
#include <boost/noncopyable.hpp>

namespace qi {
  class AnimationModelPrivate;
  class ActuatorListModel;

  class QICORECOMPAT_API AnimationModel : private boost::noncopyable {
  public:
    enum MotionResourcesHandler
    {
      MotionResourcesHandler_Passive,
      MotionResourcesHandler_Waiting,
      MotionResourcesHandler_Aggressive
    };

    AnimationModel(const std::string &path,
                   int fps = 25,
                   int startFrame = 0,
                   int endFrame = -1,
                   MotionResourcesHandler resources = MotionResourcesHandler_Passive,
                   int size = 0,
                   const std::string &formatVersion = "4",
                   boost::shared_ptr<ActuatorListModel> actuatorList = boost::shared_ptr<ActuatorListModel>());
    virtual ~AnimationModel();

    std::string path() const;
    int fps() const;
    int startFrame() const;
    int endFrame() const;
    int size() const;
    MotionResourcesHandler resourcesAcquisition() const;
    const std::string& formatVersion() const;
    boost::shared_ptr<ActuatorListModel> actuatorList() const;

    void setPath(const std::string &path);
    void setFPS(int fps);
    void setStartFrame(int start_frame);
    void setEndFrame(int end_frame);
    void setResourcesAcquisition(MotionResourcesHandler resources);
    void setSize(int size);
    void setFormatVersion(const std::string &format_version);
    void setActuatorList(boost::shared_ptr<ActuatorListModel> actuator_list);

    bool loadFromFile();

  private:
    AnimationModelPrivate* _p;
  };
  typedef boost::shared_ptr<AnimationModel> AnimationModelPtr;
}

#endif /* !ANIMATION_H_ */
