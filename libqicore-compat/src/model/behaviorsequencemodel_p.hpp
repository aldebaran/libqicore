/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef BEHAVIORSEQUENCE_P_H_
# define BEHAVIORSEQUENCE_P_H_

# include <string>
# include <list>

# include <qicore-compat/model/behaviorlayermodel.hpp>

namespace qi
{
  class BehaviorSequenceModelPrivate
  {
    friend class BehaviorSequenceModel;
  public:
    BehaviorSequenceModelPrivate(const std::string &path,
                                 int fps,
                                 int startFrame,
                                 int endFrame,
                                 int size,
                                 const std::string &formatVersion );

    bool loadFromFile();

  private:
    std::string _path;
    int _fps;
    int _startFrame;
    int _endFrame;
    int _size;
    std::string _formatVersion;
    std::list<BehaviorLayerModelPtr> _behaviorsLayer;
  };
}

#endif /* !BEHAVIORSEQUENCE_P_H_ */
