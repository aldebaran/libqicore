/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef ANIMATION_P_H_
#define ANIMATION_P_H_


#include <string>
#include <boost/filesystem.hpp>

#include <qicore-compat/model/actuatorlistmodel.hpp>

namespace qi
{
  class AnimationModelPrivate
  {
    friend class AnimationModel;
  public:
    AnimationModelPrivate(const std::string &path,
                          int fps,
                          int startFrame,
                          int endFrame,
                          int size,
                          const std::string &formatVersion,
                          ActuatorListModelPtr actuatorList);

    bool loadFromFile();

  private:
    std::string _path;
    int _fps;
    int _startFrame;
    int _endFrame;
    int _size;
    std::string _formatVersion;
    ActuatorListModelPtr _actuatorList;

  };
}

#endif /* !ANIMATION_P_H_ */
